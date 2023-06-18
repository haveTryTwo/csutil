// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_QUEUE_H_
#define MY_QUEUE_H_

#include "my_deque.h"

template <class T, class Sequence = my_deque<T> >
class my_queue {
 private:
  template <class T1, class Sequence1>
  friend bool operator==(const my_queue<T1, Sequence1>&, const my_queue<T1, Sequence1>&);

  template <class T1, class Sequence1>
  friend bool operator<(const my_queue<T1, Sequence1>&, const my_queue<T1, Sequence1>&);

 public:
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::reference reference;
  typedef typename Sequence::const_reference const_reference;

 protected:
  Sequence c;

 public:
  bool empty() const { return c.empty(); }

  size_type size() const { return c.size(); }

  reference front() { return c.front(); }

  const_reference front() const { return c.front(); }

  reference back() { return c.back(); }

  const_reference back() const { return c.back(); }

  void push(const value_type& x) { c.push_back(x); }

  void pop() { c.pop_front(); }
};

template <class T, class Sequence>
bool operator==(const my_queue<T, Sequence>& x, const my_queue<T, Sequence>& y) {
  return x.c == y.c;
}

template <class T, class Sequence>
bool operator<(const my_queue<T, Sequence>& x, const my_queue<T, Sequence>& y) {
  return x.c < y.c;
}

#endif
