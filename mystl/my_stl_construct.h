// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_CONSTRUCT_H_
#define MY_STL_CONSTRUCT_H_

// #include <new.h>

#include "my_type_traits.h"

// construct
template <class T1, class T2>
inline void my_construct(T1* p, const T2& value) {
  new (p) T1(value);
}

// destroy
template <class T>
inline void my_destroy(T* pointer) {
  pointer->~T();
}

template <class ForwardIterator>
inline void my_destroy(ForwardIterator first, ForwardIterator last) {
  __my_destroy(first, last, value_type(first));
}

template <class ForwardIterator, class T>
inline void __my_destroy(ForwardIterator first, ForwardIterator last, T*) {
  typedef typename __my_type_traits<T>::has_trivial_destructor trivial_destructor;
  __my_destroy_aux(first, last, trivial_destructor());
}

template <class ForwardIterator>
inline void __my_destroy_aux(ForwardIterator first, ForwardIterator last, __my_false_type) {
  for (; first != last; ++first) {
    my_destroy(&*first);
  }
}

template <class ForwardIterator>
inline void __my_destroy_aux(ForwardIterator first, ForwardIterator last, __my_true_type) {}

inline void my_destroy(char*, char*) {}

inline void my_destroy(wchar_t*, wchar_t*) {}

#endif
