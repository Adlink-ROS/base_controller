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

#include <memory>

#include <serial/serial.h>

#include "base_controller/controllers.hpp"

/*
 * STMController
 */

/* There will be only one serial device when interactive with the STMController */
std::shared_ptr<serial::Serial> unique_serial;

void STMController::init(void)
{
  this->running = false;

  // default ring buffer size = 2048
  this->ringbuf =
    std::make_unique<circular_buffer<uint8_t>>(2048);
}

void STMController::deinit(void)
{
  this->running = false;
  // Wait until finished
  this->recv_thread.join();
}

void STMController::runner(void)
{
  size_t chunk_size = 4;
  size_t return_size = 0;
  std::vector<uint8_t> _local_buf(chunk_size);

  while (this->running.load())
  {
    return_size = unique_serial->read(_local_buf, chunk_size);
    for (auto i = 0; i < return_size; i++) {
      this->ringbuf->put(_local_buf.at(i));
    }
  }
}

void STMController::consumer(void)
{
  while (this->running.load())
  {
    uint8_t chr = this->ringbuf->get();
  }
}
