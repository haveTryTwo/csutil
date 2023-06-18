// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_STL_ALLOC_H_
#define MY_STL_ALLOC_H_

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC                    \
  std::cerr << "out of memory" << std::endl; \
  exit(1)
#endif

#include <cstdlib>

template <int inst>
class __my_malloc_alloc_template {
 private:
  // the situation is out of memory
  static void *oom_malloc(size_t);
  static void *oom_realloc(void *, size_t);
  static void (*__malloc_alloc_oom_handler)();

 public:
  // allocate the space
  static void *allocate(size_t n) {
    void *result = malloc(n);
    if (NULL == result) {
      // when it is not satisfied, call function oom_malloc()
      result = oom_malloc(n);
    }
    return result;
  }

  // deallocate
  static void deallocate(void *p, size_t) { free(p); }

  // reallocate the space
  static void *reallocate(void *p, size_t /*old_sz*/, size_t new_sz) {
    void *result = realloc(p, new_sz);
    if (0 == result) {
      // when it's not satisfied, call function oom_realloc();
      result = oom_realloc(p, new_sz);
    }
    return result;
  }

  // set_new_handler()
  static void (*set_malloc_handler(void (*f)()))() {
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;  // set new handler
    return (old);
  }
};

// malloc_alloc
template <int inst>
void (*__my_malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

// oom_malloc
template <int inst>
void *__my_malloc_alloc_template<inst>::oom_malloc(size_t n) {
  void (*my_malloc_handler)();
  void *result;

  for (;;) {
    // no stop to test to alloc the memory
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) {
      __THROW_BAD_ALLOC;
    }
    (*my_malloc_handler)();
    result = malloc(n);

    if (result) {
      return (result);
    }
  }
}

// oom_realloc
template <int inst>
void *__my_malloc_alloc_template<inst>::oom_realloc(void *p, size_t n) {
  void (*my_malloc_handler)();
  void *result = NULL;

  for (;;) {
    // no stop to test to realloc the memory
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) {
      __THROW_BAD_ALLOC;
    }
    (*my_malloc_handler)();
    result = realloc(p, n);

    if (result != NULL) {
      return (result);
    }
  }
}

#endif
