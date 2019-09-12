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

#ifndef BASE_CONTROLLER_CONTROLLERS_HPP_
#define BASE_CONTROLLER_CONTROLLERS_HPP_

#include <thread>
#include <atomic>
#include <memory>

#include "base_controller/base_controller.hpp"
#include "base_controller/ringbuffer.hpp"

class STMController : public base_controller
{
  public:
    STMController(std::string name, uint32_t baudrate);
    ~STMController();
    int8_t init(void);
    void deinit(void);
    void runner(void);
    void consumer(void);
  private:
    std::thread recv_thread;
    std::atomic_bool running;
    std::string port_name;
    uint32_t baudrate;
    /* circular_buffer is thread-safe */
    std::unique_ptr<circular_buffer<uint8_t>> ringbuf;
};

#endif /* BASE_CONTROLLER_CONTROLLERS_HPP_ */
