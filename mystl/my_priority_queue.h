// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_PRIORITY_QUEUE_H_
#define MY_PRIORITY_QUEUE_H_

#include "my_heap.h"
#include "my_vector.h"

template <class T, class Sequence = my_vector<T> >
class my_priority_queue {
 public:
  typedef typename Sequence ::value_type value_type;
  typedef typename Sequence ::size_type size_type;
  typedef typename Sequence ::reference reference;
  typedef typename Sequence ::const_reference const_reference;

 protected:
  Sequence c;

 public:
  my_priority_queue() : c() {}

  template <class InputIterator>
  my_priority_queue(InputIterator first, InputIterator last) : c(first, last) {
    my_make_heap(c.begin(), c.end());
  }

  bool empty() const { return c.empty(); }

  size_type size() const { return c.size(); }

  const_reference top() const { return c.front(); }

  void push(const value_type& x) {
    c.push_back(x);
    my_push_heap(c.begin(), c.end());
  }

  void pop() {
    my_pop_heap(c.begin(), c.end());
    c.pop_back();
  }
};

#endif
