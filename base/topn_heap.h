// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_MUTABLE_BUFFER_H_
#define SOCK_MUTABLE_BUFFER_H_

#include <stdint.h>

#include <algorithm>
#include <vector>

#include "base/status.h"

namespace base {

template <class T, class HeapCompare>
class TopNHeap { /*{{{*/
 public:
  TopNHeap(uint32_t top_num);
  virtual ~TopNHeap();

 public:
  Code Insert(const T &value);
  Code Insert(const std::vector<T> &values);

  Code Pop(std::vector<T> *result);

 private:
  uint32_t top_num_;  // top n number
  std::vector<T> buf_;
}; /*}}}*/

template <class T, class HeapCompare>
TopNHeap<T, HeapCompare>::TopNHeap(uint32_t top_num) : top_num_(top_num) { /*{{{*/
  std::make_heap(buf_.begin(), buf_.end(), HeapCompare());
} /*}}}*/

template <class T, class HeapCompare>
TopNHeap<T, HeapCompare>::~TopNHeap() { /*{{{*/
} /*}}}*/

template <class T, class HeapCompare>
Code TopNHeap<T, HeapCompare>::Insert(const T &value) { /*{{{*/
  if (buf_.size() >= top_num_) {
    if (HeapCompare()(buf_.front(), value)) {
      return kOk;  // No need modify
    }

    std::pop_heap(buf_.begin(), buf_.end(), HeapCompare());
    buf_.pop_back();
  }

  buf_.push_back(value);
  std::push_heap(buf_.begin(), buf_.end(), HeapCompare());
  // for (int i = 0; i < buf_.size(); ++i)
  // {
  //     fprintf(stderr, "%d\n", (int)buf_[i]);
  // }
  // fprintf(stderr, "\n");

  return kOk;
} /*}}}*/

template <class T, class HeapCompare>
Code TopNHeap<T, HeapCompare>::Insert(const std::vector<T> &values) { /*{{{*/
  typename std::vector<T>::const_iterator const_it = values.begin();
  for (const_it = values.begin(); const_it != values.end(); ++const_it) {
    Code ret = Insert(*const_it);
    if (ret != kOk) return ret;
  }

  return kOk;
} /*}}}*/

template <class T, class HeapCompare>
Code TopNHeap<T, HeapCompare>::Pop(std::vector<T> *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  std::sort_heap(buf_.begin(), buf_.end(), HeapCompare());

  typename std::vector<T>::const_iterator const_it = buf_.begin();
  for (const_it = buf_.begin(); const_it != buf_.end(); ++const_it) {
    result->push_back(*const_it);
  }

  buf_.clear();

  return kOk;
} /*}}}*/

template <class T>
class MaxTopNHeap : public TopNHeap<T, std::greater<T> > {
 public:
  MaxTopNHeap(uint32_t max_num = 10) : TopNHeap<T, std::greater<T> >(max_num) {}
  virtual ~MaxTopNHeap() {}
};

template <class T>
class MinTopNHeap : public TopNHeap<T, std::less<T> > {
 public:
  MinTopNHeap(uint32_t max_num = 10) : TopNHeap<T, std::less<T> >(max_num) {}
  virtual ~MinTopNHeap() {}
};

// typedef template<class T>  TopNHeap<T, std::greater<T>, std::less<T> >  MaxTopNHeap;
// typedef template<class T> TopNHeap<T, std::less, std::greater>  MinTopNHeap;

}  // namespace base

#endif
