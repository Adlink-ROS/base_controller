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

#ifndef BASE_CONTROLLER_RINGBUFFER_HPP_
#define BASE_CONTROLLER_RINGBUFFER_HPP_

#include <memory>
#include <mutex>

template <class T>
class circular_buffer {
  public:
    explicit circular_buffer(size_t size) :
      buf_(std::unique_ptr<T[]>(new T[size])),
      max_size_(size)
  { // empty
  }

    void put(T item);
    T get();
    void reset();
    bool empty() const;
    bool full() const;
    size_t capacity() const;
    size_t size() const;

  private:
    std::mutex mutex_;
    std::unique_ptr<T[]> buf_;
    size_t head_ = 0;
    size_t tail_ = 0;
    const size_t max_size_;
    bool full_ = 0;
};

#endif /* BASE_CONTROLLER_RINGBUFFER_HPP_ */
