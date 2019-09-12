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

#include <memory>
#include <iostream>

#include <serial/serial.h>

#include "base_controller/controllers.hpp"

/*
 * STMController
 */

/* There will be only one serial device when interactive with the STMController */
std::shared_ptr<serial::Serial> serial_device = nullptr;

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

  this->running = false;

  // default ring buffer size = 2048
  this->ringbuf =
    std::make_unique<circular_buffer<uint8_t>>(2048);

  this->recv_thread = std::thread(
      [this]()->void {
      this->runner();
      });

  return ret;
}

void STMController::deinit(void)
{
  this->running = false;
  // Wait until finished
  if (this->recv_thread.joinable())
  {
    this->recv_thread.join();
  }
}

void STMController::runner(void)
{
  size_t chunk_size = 4;
  size_t return_size = 0;
  std::vector<uint8_t> _local_buf(chunk_size);

  while (this->running.load())
  {
    return_size = serial_device->read(_local_buf, chunk_size);
    for (size_t i = 0; i < return_size; i++) {
      this->ringbuf->put(_local_buf.at(i));
    }
  }
}

void STMController::consumer(void)
{
  while (this->running.load())
  {
    //uint8_t chr = this->ringbuf->get();
  }
}
