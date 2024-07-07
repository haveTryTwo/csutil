// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SEARCH_H_
#define BASE_SEARCH_H_

#include <stdio.h>

#include "base/common.h"
#include "base/status.h"

namespace base {

template <typename T>
int CompareNum(const void *first, const void *second) {
  return (*(T *)first - *(T *)second);
}

// std::pair<std::string, std::string>
template <typename Pair>
int ComparePair(const void *first, const void *second) {
  return ((Pair *)first)->first.compare(((Pair *)second)->first);
}

/**
 * Note: 1. this search is not precisely searching, it's range searching
 *          for example, an array : {40, 30, 20, 10}, here 20 means [20, 30), if you find 25,
 *          then it will return 20 for you, as the same, if you find 41, you will get 40;
 *
 *       2. This is usefull when it is index infomation, each index points to one data
 *          block which has not only one data;
 *
 *       3. The array is sorted descending
 */
template <typename T>
Code BinaryDescendSearch(const T *arr, int size, const T &data, int *pos, Compare comp) { /*{{{*/
  if (size <= 0 || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = size - 1;

  while (low < high) {
    mid = (low + high) / 2;
    if (comp(arr + mid, &data) > 0)
      low = mid + 1;
    else if (comp(arr + mid, &data) < 0)
      high = mid;
    else {
      low = mid;
      break;
    }
  }

  *pos = low;
  return kOk;
} /*}}}*/

/**
 * Note: 1. this function is same to BinaryDescendSearch except the array is sorted ascending
 */
template <typename T>
Code BinaryAscendSearch(const T *arr, int size, const T &data, int *pos, Compare comp) { /*{{{*/
  if (size <= 0 || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = size - 1;

  while (low < high) {
    mid = (low + high + 1) / 2;
    if (comp(arr + mid, &data) > 0)
      high = mid - 1;
    else if (comp(arr + mid, &data) < 0)
      low = mid;
    else {
      low = mid;
      break;
    }
  }

  *pos = low;
  return kOk;
} /*}}}*/

template <typename RandomIterator, typename T>
Code BinaryDescendSearch(RandomIterator first, RandomIterator last, const T &data, RandomIterator *pos,
                         Compare comp) { /*{{{*/
  if (first > last || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = last - first - 1;

  while (low < high) {
    mid = (low + high) / 2;
    if (comp(&(*(first + mid)), &data) > 0)
      low = mid + 1;
    else if (comp(&(*(first + mid)), &data) < 0)
      high = mid;
    else {
      low = mid;
      break;
    }
  }

  *pos = first + low;
  return kOk;
} /*}}}*/

template <typename RandomIterator, typename T>
Code BinaryAscendSearch(RandomIterator first, RandomIterator last, const T &data, RandomIterator *pos,
                        Compare comp) { /*{{{*/
  if (first > last || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = last - first - 1;

  while (low < high) {
    mid = (low + high + 1) / 2;
    if (comp(&(*(first + mid)), &data) > 0)
      high = mid - 1;
    else if (comp(&(*(first + mid)), &data) < 0)
      low = mid;
    else {
      low = mid;
      break;
    }
  }

  *pos = first + low;
  return kOk;
} /*}}}*/

template <typename RandomIterator>
void Print(RandomIterator first, RandomIterator last, int type) { /*{{{*/
  fprintf(stderr, "{ ");
  for (; first != last; ++first) {
    fprintf(stderr, "%d, ", *first);
  }
  fprintf(stderr, " }\n");
} /*}}}*/

}  // namespace base

#endif
