// Copyright 2019 ADLINK Technology Ltd. Advanced Robotic Platform Group
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// reference: https://embeddedartistry.com/blog/2017/4/6/circular-buffers-in-cc#api-design
// reference: https://thispointer.com/c11-how-to-use-stdthread-as-a-member-variable-in-class/

#include <iostream>
#include <memory>
#include <chrono>
#include <bitset>

#include <serial/serial.h>

#include "base_controller/controllers.hpp"

/*
 * STMController
 */

/* There will be only one serial device when interactive with the STMController */
static std::shared_ptr<serial::Serial> serial_device = nullptr;

STMController::STMController(std::string name, uint32_t baudrate) :
  port_name(name), baudrate(baudrate)
{

}

STMController::~STMController()
{

}

int8_t STMController::init(void)
{
  int8_t ret = -1;
  try {
    if (serial_device == nullptr) {
      serial_device = std::make_shared<serial::Serial>
        (this->port_name, this->baudrate, serial::Timeout::simpleTimeout(1000));
    }
  } catch (const std::exception& ex) {
    throw;
  }

  if (!serial_device->isOpen()) {
    std::cout << "device is not open" << std::endl;
    throw;
  }

  this->running = false;

  // default ring buffer size = 2048
  this->ringbuf =
    std::make_shared<circular_buffer<uint8_t>>(2048);

  this->running = true;

  this->running_thread = std::thread(
      [this]()->void {
      this->runner();
      });

  this->consumer_thread = std::thread(
      [this]()->void {
      this->consumer();
      });

  this->commander_thread = std::thread(
      [this]()->void {
      this->commander();
      });

  return ret;
}

void STMController::deinit(void)
{
  this->running = false;
  this->ringbuf->put(0);
  // Wait until finished
  if (this->running_thread.joinable())
  {
    this->running_thread.join();
  }
  if (this->consumer_thread.joinable())
  {
    this->consumer_thread.join();
  }
  if (this->commander_thread.joinable())
  {
   this->commander_thread.join();
  }
  serial_device->close();
}

void STMController::send_cmd(std::vector<uint8_t> buffer, size_t len)
{
  controller_cmd cmd ;
  cmd.buffer = buffer;
  cmd.len = len;
  this->cmd_queue.enqueue(cmd);
}

void STMController::commander(void) {
  while (this->running.load()) {
    size_t n = 0;
    // queue is thread safe
    controller_cmd cmd = this->cmd_queue.dequeue();
    while (n < cmd.len) {
      n += serial_device->write(cmd.buffer.data()+n, cmd.len-n);
    }
  }
}

void STMController::runner(void)
{
  size_t return_size = 0;
  uint8_t chr = 0;
  while (this->running.load())
  {
    return_size = serial_device->read(&chr, 1);
    if (return_size == 1)
      this->ringbuf->put(chr);
  }
}

void STMController::consumer(void)
{
  size_t payload_size = 0;
  uint8_t event_code = 0;
  std::vector<uint8_t> payload_vector;
  while (this->running.load())
  {
    uint8_t first_chr = this->ringbuf->get();
    if(first_chr != 0xFF)
      continue;
    uint8_t second_chr = this->ringbuf->get();
    if (second_chr == 0xFA) {
      payload_size = 12; event_code = 0xFA;
    } else if (second_chr == 0xFB) {
      payload_size = 7; event_code = 0xFB;
    } else if (second_chr == 0xFC) {
      payload_size = 13; event_code = 0xFC;
    } else {
      // Do nothing when not match, continue to the next loop
      continue;
    }
    /*
     * vector.resize() allocated a new memory here,
     * which might cause performance issue
     */
    payload_vector.resize(payload_size);
    for (auto it = payload_vector.begin();
        it != payload_vector.end(); ++it) {
      *it = this->ringbuf->get();
    }
    if (this->event_map.find(event_code) ==
        this->event_map.end())
    {
      // Do nothing when not matching
    } else {
      auto event = this->event_map[event_code];
      event.cb(payload_vector);
    }
  }
}