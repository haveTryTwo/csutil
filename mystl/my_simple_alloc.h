// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_SIMPLE_ALLOC_H_
#define MY_SIMPLE_ALLOC_H_

#include "my_default_stl_alloc.h"
#include "my_stl_alloc.h"

#define __NODE_ALLOCATOR_THREADS true

#ifdef __USE_MALLOC
typedef my_malloc_alloc my_alloc;
#else
typedef __my_default_alloc_template<__NODE_ALLOCATOR_THREADS, 0> my_alloc;
#endif

template <class T, class Alloc>
class my_simple_alloc {
 public:
  static T* allocate(size_t n) { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }

  static T* allocate(void) { return (T*)Alloc ::allocate(sizeof(T)); }

  static void deallocate(T* p, size_t n) {
    if (0 != n) {
      Alloc ::deallocate(p, n * sizeof(T));
    }
  }

  static void deallocate(T* p) { Alloc ::deallocate(p, sizeof(T)); }
};

#endif
