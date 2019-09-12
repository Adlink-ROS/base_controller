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

struct controller_event {
  std::string name;
  std::string description;
  std::function<void()> cb;
  std::function<bool()> matching_func;
};

class base_controller {
  public:
    void notify(controller_event & event);
    void register_event(controller_event & event) {
      this->event_map.insert
        (std::pair<std::string, controller_event>(event.name, event));
    }
    void unregister_event(std::string name) {
      auto it = this->event_map.find(name);
      this->event_map.erase(it);
    }
    virtual int8_t init(void) = 0;
    virtual void deinit(void) = 0;
  private:
    std::map<std::string, controller_event> event_map =
      std::map<std::string, controller_event>();
};

#endif /* BASE_CONTROLLER_HPP_ */
