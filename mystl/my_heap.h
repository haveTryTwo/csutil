// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MY_HEAP_H_
#define MY_HEAP_H_

#include "my_vector.h"

// push the heap, the value has been push in the last
template <class RandomAccessIterator>
inline void my_push_heap(RandomAccessIterator first, RandomAccessIterator last) {
  __my_push_heap_aux(first, last, distance_type(first), value_type(first));
}

template <class RandomAccessIterator, class Distance, class T>
inline void __my_push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*,
                               T*) {
  __my_push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

// change the position, so that the value can be suitable for the heap
template <class RandomAccessIterator, class Distance, class T>
void __my_push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
  Distance parent = (holeIndex - 1) / 2;
  while (holeIndex > topIndex && *(first + parent) < value) {
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

// make the root value in the last position of the container, and sort the containter to be just
// like a heap except the last which is the root value
template <class RandomAccessIterator>
inline void my_pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
  __my_pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __my_pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*) {
  __my_pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
inline void __my_pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator result, T value, Distance*) {
  *result = *first;
  *first = value;
  __my_adjust_heap(first, Distance(0), Distance(last - first), value);
}

// adjust the container, so make the container be more like a heap
template <class RandomAccessIterator, class Distance, class T>
void __my_adjust_heap(
    RandomAccessIterator first, Distance holeIndex, Distance len,
    T value) {  // NOTE: empe, shiftup and shiftdown for heap  is replaced by this method
  Distance topIndex = holeIndex;
  Distance secondIndex = 2 * holeIndex + 2;
  while (secondIndex < len) {
    if (*(first + secondIndex) < *(first + secondIndex - 1)) {
      secondIndex--;
    }
    if (*(first + holeIndex) >= *(first + secondIndex)) {
      // if the abstract root value is bigger than its child's value, then return
      return;
    }
    my_swap(*(first + holeIndex), *(first + secondIndex));

    holeIndex = secondIndex;
    secondIndex = 2 * holeIndex + 2;
  }

  if (secondIndex == len) {
    if (*(first + holeIndex) < *(first + secondIndex - 1)) {
      my_swap(*(first + holeIndex), *(first + secondIndex - 1));
    }
  }
}

// make the container be a heap, the heap here is just like a algorhtim, and its function is making
// the value in the container be in a type that the parent's value is bigger than the children's
// value
template <class RandomAccessIterator>
inline void my_make_heap(RandomAccessIterator first, RandomAccessIterator last) {
  __my_make_heap(first, last, value_type(first), distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
void __my_make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
  if (last - first < 2) {
    return;
  }

  Distance len = last - first;
  Distance parent = (len - 2) / 2;

  while (true) {
    __my_adjust_heap(first, parent, len, T(*(first + parent)));
    if (parent == 0) {
      return;
    }
    parent--;
  }
}

// the heap must already be a heap, then my_sort_heap will make the container be sorted
// so the result will be ascend
template <class RandomAccessIterator>
void my_sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
  while (last - first > 1) {
    my_pop_heap(first, last--);
  }
}

#endif
