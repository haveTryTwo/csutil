// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_DEFAULT_STL_ALLOC_H_
#define MY_DEFAULT_STL_ALLOC_H_

#include <iostream>

#include <stdlib.h>

#include "my_stl_alloc.h"

typedef __my_malloc_alloc_template<0> my_malloc_alloc;

// the next setting is the least and most size of the block
enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };  // number of the free-lists

//
template <bool threads, int inst>
class __my_default_alloc_template {
 private:
  // round_up the size of block to be the of 8
  static size_t ROUND_UP(size_t bytes) { return ((bytes) + __ALIGN - 1) & ~(__ALIGN - 1); }

 private:
  // the node of free-list
  union obj {
    union obj *free_list_link;  //
    char client_data[1];        // client sees this
  };

 private:
  static union obj *volatile free_list[__NFREELISTS];

  static size_t FREELIST_INDEX(size_t bytes) {
    // to get the index of the block accoding to the size
    return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
  }

  // return an obj that size of n, and maybe it will be added to free_list
  static void *refill(size_t n);

  // to alloc a block , which can contains nobjs of objects that size is the next size
  static char *chunk_alloc(size_t size, int &nobjs);

  // Chunk allocation state
  static char *start_free;  // the start of memory pool, and only change in chunk_alloc
  static char *end_free;    // the end of memory pool, and only change in chunk_alloc
  static size_t heap_size;  //

 public:
  static void *allocate(size_t n);
  static void deallocate(void *p, size_t n);
  static void *reallocate(void *p, size_t old_sz, size_t new_sz);
};

template <bool threads, int inst>
char *__my_default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__my_default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __my_default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __my_default_alloc_template<threads, inst>::obj
    *volatile __my_default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

template <bool threads, int inst>
void *__my_default_alloc_template<threads, inst>::allocate(size_t n) {
  obj *volatile *my_free_list;
  obj *result = NULL;

  // bigger than 128 bytes
  if (n > (size_t)__MAX_BYTES) {
    return (my_malloc_alloc::allocate(n));
  }

  // look for a suitable in 16 numbers of the list
  my_free_list = free_list + FREELIST_INDEX(n);
  result = *my_free_list;

  if (result == NULL) {
    // no free_list
    void *r = refill(ROUND_UP(n));
    return r;
  }

  *my_free_list = result->free_list_link;
  return (result);
}

template <bool threads, int inst>
void __my_default_alloc_template<threads, inst>::deallocate(void *p, size_t n) {
  obj *q = (obj *)p;
  obj *volatile *my_free_list;

  // bigger than 128 bytes
  if (n > (size_t)__MAX_BYTES) {
    my_malloc_alloc::deallocate(p, n);
    return;
  }

  // look for free_list
  my_free_list = free_list + FREELIST_INDEX(n);
  // adjust for free list
  q->free_list_link = *my_free_list;
  *my_free_list = q;
}

template <bool threads, int inst>
void *__my_default_alloc_template<threads, inst>::reallocate(void *p, size_t old_sz,
                                                             size_t new_sz) {
  obj *volatile *my_free_list;
  obj *result;

  // bigger than 128 bytes
  if (new_sz > (size_t)__MAX_BYTES) {
    return my_malloc_alloc::reallocate(p, old_sz, new_sz);
  }

  // if the new size is equal with the old size,then return p
  if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) {
    return p;
  }
  // alloc the size
  result = allocate(new_sz);
  size_t size = new_sz > old_sz ? old_sz : new_sz;
  memcpy(result, p, size);
  deallocate(p, old_sz);

  return result;
}

template <bool threads, int inst>
void *__my_default_alloc_template<threads, inst>::refill(size_t n) {
  // allocate 20 objs
  int nobjs = 20;

  char *chunk = chunk_alloc(n, nobjs);
  obj *volatile *my_free_list = NULL;
  obj *result = NULL;
  obj *current_obj, 
  obj *next_obj = NULL;
  int i = 0;

  // if only one block, then return
  if (1 == nobjs) {
    return chunk;
  }

  // else add the external blocks to the free_list
  my_free_list = free_list + FREELIST_INDEX(n);
  result = (obj *)chunk;
  *my_free_list = next_obj = (obj *)(chunk + n);

  for (i = 1;; i++) {
    current_obj = next_obj;
    next_obj = (obj *)((char *)next_obj + n);
    if (nobjs - 1 == i) {
      // if arrive the end
      current_obj->free_list_link = 0;
      break;
    } else {
      //
      current_obj->free_list_link = next_obj;
    }
  }

  return result;
}

// alloc the momery block for the free_list
// this is the base of other alloc
template <bool threads, int inst>
char *__my_default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs) {
  char *result;
  size_t total_bytes = size * nobjs;
  size_t bytes_left = end_free - start_free;  // left in the memory

  if (bytes_left >= total_bytes) {
    // if the left size is bigger enough for the size
    result = start_free;
    start_free += total_bytes;
    return result;
  } else if (bytes_left >= size) {
    // if the left size is at least one size
    nobjs = bytes_left / size;
    total_bytes = size * nobjs;
    result = start_free;
    start_free += total_bytes;
    return result;
  } else {
    // if the left size is even less than one size
    size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
    if (bytes_left > 0) {
      // if the left size isn't null, first alloc the size to suitable free_list
      obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
      ((obj *)start_free)->free_list_link = *my_free_list;
      *my_free_list = (obj *)start_free;
    }

    // to get the size
    start_free = (char *)malloc(bytes_to_get);
    if (0 == start_free) {
      // heap isnot enough , malloc is not success
      int i;
      obj *volatile *my_free_list, *p;
      for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
        // try to get from the free list
        my_free_list = free_list + FREELIST_INDEX(i);
        p = *my_free_list;
        if (0 != p) {
          *my_free_list = p->free_list_link;
          start_free = (char *)p;
          end_free = start_free + i;

          // to adjust self, and adjut nobjs
          return (chunk_alloc(size, nobjs));
        }
      }
      end_free = 0;  // no memory
      start_free = (char *)my_malloc_alloc::allocate(bytes_to_get);
      // this can lead to change in memory that is not enough, or lead to exception
    }

    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    // adjust selt to change nobjs
    return (chunk_alloc(size, nobjs));
  }
}
#endif
