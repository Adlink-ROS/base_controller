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

#include "base_controller/ringbuffer.hpp"

template <class T>
void circular_buffer<T>::reset()
{
  std::lock_guard<std::mutex> lock(mutex_);
  this->head_ = this->tail_;
  this->full_ = false;
}

template <class T>
bool circular_buffer<T>::empty() const
{
  return (!this->full_ && (this->head_ == this->tail_));
}

template <class T>
bool circular_buffer<T>::full() const
{
  return this->full_;
}

template <class T>
size_t circular_buffer<T>::capacity() const
{
  return this->max_size_;
}

template <class T>
size_t circular_buffer<T>::size() const
{
  size_t size = this->max_size_;

  if(!this->full_)
  {
    if(this->head_ >= this->tail_)
    {
      size = this->head_ - this->tail_;
    }
    else
    {
      size = this->max_size_ + this->head_ - this->tail_;
    }
  }

  return size;
}

template <class T>
void circular_buffer<T>::put(T item)
{
  std::lock_guard<std::mutex> lock(mutex_);

  this->buf_[this->head_] = item;

  if(this->full_)
  {
    this->tail_ = (this->tail_ + 1) % this->max_size_;
  }

  this->head_ = (this->head_ + 1) % this->max_size_;

  this->full_ = this->head_ == this->tail_;
}

template <class T>
T circular_buffer<T>::get()
{
  std::lock_guard<std::mutex> lock(mutex_);

  if(this->empty())
  {
    return T();
  }

  //Read data and advance the tail (we now have a free space)
  auto val = this->buf_[this->tail_];
  this->full_ = false;
  this->tail_ = (this->tail_ + 1) % this->max_size_;

  return val;
}
