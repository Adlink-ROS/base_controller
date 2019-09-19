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

#include <mutex>
#include <condition_variable>
#include <queue>

template <class T>
class SafeQueue
{
  public:
    SafeQueue(void)
      : q(), m(), c()
    {}

    ~SafeQueue(void)
    {}

    // Add an element to the queue.
    void enqueue(T t)
    {
      std::lock_guard<std::mutex> lock(m);
      q.push(t);
      c.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
      std::unique_lock<std::mutex> lock(m);
      while(q.empty())
      {
        // release lock as long as the wait and reaquire it afterwards.
        c.wait(lock);
      }
      T val = q.front();
      q.pop();
      return val;
    }

  private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};
