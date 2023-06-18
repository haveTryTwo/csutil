// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_PAIR_H_
#define MY_PAIR_H_

/**
 * problem: the type translate, look for this and refer to stl_pair.h and move.h and so on
 */

template <class T1, class T2>
class my_pair {
 public:
  typedef T1 first_type;
  typedef T2 second_type;

  T1 first;
  T2 second;

  my_pair() : first(T1()), second(T2()) {}

  my_pair(const T1& a, const T2& b) : first(a), second(b) {}

  my_pair(const my_pair& p) : first(p.first), second(p.second) {}

  /**
   * This copy construct function can make the difference type to be the type that this class type
   * needs
   */
  template <class U1, class U2>
  my_pair(my_pair<U1, U2>& p) : first(p.first), second(p.second) {}

  my_pair& operator=(const my_pair& p) {
    if (this == &p) {
      return *this;
    }
    first = p.first;
    second = p.second;
    return *this;
  }

  /**
   * This assignment construct function can make the difference type to be the type that this class
   * type needs
   */
  template <class U1, class U2>
  my_pair& operator=(my_pair<U1, U2>& p) {
    if ((void*)this == (void*)&p) {
      return *this;
    }
    first = p.first;
    second = p.second;
    return *this;
  }

  template <class _T1, class _T2>
  friend bool operator==(const my_pair<_T1, _T2>& p1, const my_pair<_T1, _T2>& p2);
};

template <class T1, class T2>
bool operator==(const my_pair<T1, T2>& p1, const my_pair<T1, T2>& p2) {
  return p1.first == p2.first && p1.second == p2.second;
}

template <class T1, class T2>
my_pair<T1, T2> my_make_pair(const T1& x, const T2& y) {
  return my_pair<T1, T2>(x, y);
}

#endif
