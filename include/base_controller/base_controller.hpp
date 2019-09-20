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

#ifndef BASE_CONTROLLER_HPP_
#define BASE_CONTROLLER_HPP_

#include <string>
#include <map>
#include <functional>
#include <vector>

struct controller_event {
  std::string name;
  std::string description;
  uint8_t event_code;
  std::function<void(std::vector<uint8_t>)> cb;
};

struct controller_cmd {
  size_t len;
  uint8_t * buffer;
};

class base_controller {
  public:
    void notify(controller_event & event);
    void register_event(controller_event & event) {
      this->event_map.insert
        (std::pair<uint8_t, controller_event>(event.event_code, event));
    }
    void unregister_event(uint8_t event_code) {
      auto it = this->event_map.find(event_code);
      this->event_map.erase(it);
    }
    virtual int8_t init(void) = 0;
    virtual void deinit(void) = 0;
    virtual void send_cmd(uint8_t * buffer, size_t len) = 0;
    std::map<uint8_t, controller_event> event_map =
      std::map<uint8_t, controller_event>();
};

#endif /* BASE_CONTROLLER_HPP_ */
