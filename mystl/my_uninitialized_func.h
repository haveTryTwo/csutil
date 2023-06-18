// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_UNINITIALIZED_FUNC_H_
#define MY_UNINITIALIZED_FUNC_H_

#include "my_stl_algobase.h"
#include "my_stl_construct.h"

#include <cstring>

/* the next functions are about: unintialized_fill_n */
template <class ForwardIterator, class Size, class T>
inline ForwardIterator my_uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
  return __my_uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __my_uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
  typedef typename __my_type_traits<T1>::is_POD_type is_POD;
  return __my_uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __my_uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x,
                                                     __my_true_type) {
  return my_fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __my_uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x,
                                                     __my_false_type) {
  ForwardIterator cur = first;
  for (; n > 0; --n, ++cur) {
    my_construct(&*cur, x);
  }
  return cur;
}

/*	the next functions are about: uninitialized_fill */
template <class ForwardIterator, class T>
inline void my_uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
  __my_uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class T1>
inline void __my_uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
  typedef typename __my_type_traits<T1>::is_POD_type is_POD;
  __my_uninitialized_fill_aux(first, last, x, is_POD());
}

template <class ForwardIterator, class T>
inline void __my_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x,
                                        __my_true_type) {
  my_fill(first, last, x);
}

template <class ForwardIterator, class T>
inline void __my_uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x,
                                        __my_false_type) {
  ForwardIterator cur = first;
  for (; cur != last; ++cur) {
    my_construct(&*cur, x);
  }
}

/*  the next functions are about : uninitialized_copy */
template <class InputIterator, class ForwardIterator>
inline ForwardIterator my_uninitialized_copy(InputIterator first, InputIterator last,
                                             ForwardIterator result) {
  return __my_uninitialized_copy(first, last, result, value_type(result));
}

inline char* my_uninitialized_copy(const char* first, const char* last, char* result) {
  memmove(result, first, last - first);
  return result + (last - first);
}

inline wchar_t* my_uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __my_uninitialized_copy(InputIterator first, InputIterator last,
                                               ForwardIterator result, T*) {
  typedef typename __my_type_traits<T>::is_POD_type is_POD;
  return __my_uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __my_uninitialized_copy_aux(InputIterator first, InputIterator last,
                                                   ForwardIterator result, __my_true_type) {
  return my_copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __my_uninitialized_copy_aux(InputIterator first, InputIterator last,
                                                   ForwardIterator result, __my_false_type) {
  ForwardIterator cur = result;
  for (; first != last; ++first, ++cur) {
    my_construct(&*cur, *first);
  }
  return cur;
}

#endif
