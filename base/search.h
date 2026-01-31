// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SEARCH_H_
#define BASE_SEARCH_H_

#include <stdio.h>

#include "base/common.h"
#include "base/status.h"

namespace base {

/**
 * @brief Compare two numbers for sorting/searching
 *
 * Safe comparison function that avoids integer overflow issues.
 * Returns -1 if first < second, 0 if equal, 1 if first > second.
 *
 * @tparam T The numeric type to compare
 * @param first Pointer to the first value
 * @param second Pointer to the second value
 * @return -1, 0, or 1 based on comparison result
 */
template <typename T>
int CompareNum(const void *first, const void *second) {
  const T &a = *(const T *)first;
  const T &b = *(const T *)second;
  if (a < b) return -1;
  if (a > b) return 1;
  return 0;
}

/**
 * @brief Compare two pairs by their first element
 *
 * Typically used for std::pair<std::string, std::string> or similar types.
 * Normalizes the result to -1, 0, or 1.
 *
 * @tparam Pair The pair type to compare
 * @param first Pointer to the first pair
 * @param second Pointer to the second pair
 * @return -1, 0, or 1 based on comparison result
 */
template <typename Pair>
int ComparePair(const void *first, const void *second) {
  int result = ((const Pair *)first)->first.compare(((const Pair *)second)->first);
  if (result < 0) return -1;
  if (result > 0) return 1;
  return 0;
}

/**
 * @brief Binary search in a descending sorted array (range search)
 *
 * Note: 1. This is not precise searching, it's range searching
 *          For example, an array: {40, 30, 20, 10}, here 20 means [20, 30)
 *          If you find 25, it will return the position of 20
 *          If you find 41, it will return the position of 40
 *
 *       2. This is useful when it is index information, each index points to one data
 *          block which has not only one data
 *
 *       3. The array must be sorted in descending order
 *
 * @tparam T The type of elements in the array
 * @param arr Pointer to the array to search (must not be NULL)
 * @param size Size of the array (must be > 0)
 * @param data The value to search for
 * @param pos Output parameter for the result position
 * @param comp Comparison function
 * @return base::kOk on success, base::kInvalidParam on invalid parameters
 */
template <typename T>
Code BinaryDescendSearch(const T *arr, int size, const T &data, int *pos, Compare comp) { /*{{{*/
  if (arr == NULL || size <= 0 || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = size - 1;

  while (low < high) {
    mid = low + (high - low) / 2;  // Avoid overflow
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
 * @brief Binary search in an ascending sorted array (range search)
 *
 * Note: This function is the same as BinaryDescendSearch except the array is sorted ascending
 *
 * @tparam T The type of elements in the array
 * @param arr Pointer to the array to search (must not be NULL)
 * @param size Size of the array (must be > 0)
 * @param data The value to search for
 * @param pos Output parameter for the result position
 * @param comp Comparison function
 * @return base::kOk on success, base::kInvalidParam on invalid parameters
 */
template <typename T>
Code BinaryAscendSearch(const T *arr, int size, const T &data, int *pos, Compare comp) { /*{{{*/
  if (arr == NULL || size <= 0 || pos == NULL) return kInvalidParam;

  int low = 0;
  int mid = 0;
  int high = size - 1;

  while (low < high) {
    mid = low + (high - low + 1) / 2;  // Avoid overflow
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

/**
 * @brief Binary search in a descending sorted range using iterators
 *
 * @tparam RandomIterator Random access iterator type
 * @tparam T The type of value to search for
 * @param first Iterator to the beginning of the range
 * @param last Iterator to the end of the range (one past the last element)
 * @param data The value to search for
 * @param pos Output parameter for the result iterator position
 * @param comp Comparison function
 * @return base::kOk on success, base::kInvalidParam on invalid parameters
 */
template <typename RandomIterator, typename T>
Code BinaryDescendSearch(RandomIterator first, RandomIterator last, const T &data, RandomIterator *pos,
                         Compare comp) { /*{{{*/
  if (first > last || pos == NULL) return kInvalidParam;

  // Handle empty range
  if (first == last) {
    *pos = first;
    return kOk;
  }

  int low = 0;
  int mid = 0;
  int high = last - first - 1;

  while (low < high) {
    mid = low + (high - low) / 2;  // Avoid overflow
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

/**
 * @brief Binary search in an ascending sorted range using iterators
 *
 * @tparam RandomIterator Random access iterator type
 * @tparam T The type of value to search for
 * @param first Iterator to the beginning of the range
 * @param last Iterator to the end of the range (one past the last element)
 * @param data The value to search for
 * @param pos Output parameter for the result iterator position
 * @param comp Comparison function
 * @return base::kOk on success, base::kInvalidParam on invalid parameters
 */
template <typename RandomIterator, typename T>
Code BinaryAscendSearch(RandomIterator first, RandomIterator last, const T &data, RandomIterator *pos,
                        Compare comp) { /*{{{*/
  if (first > last || pos == NULL) return kInvalidParam;

  // Handle empty range
  if (first == last) {
    *pos = first;
    return kOk;
  }

  int low = 0;
  int mid = 0;
  int high = last - first - 1;

  while (low < high) {
    mid = low + (high - low + 1) / 2;  // Avoid overflow
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

/**
 * @brief Print elements in a range (for debugging)
 *
 * Note: This function assumes elements can be printed with %d format.
 *       For production code, consider using type-safe alternatives like iostreams.
 *
 * @tparam RandomIterator Random access iterator type
 * @param first Iterator to the beginning of the range
 * @param last Iterator to the end of the range
 * @param type Unused parameter (kept for backward compatibility)
 */
template <typename RandomIterator>
void Print(RandomIterator first, RandomIterator last, int type) { /*{{{*/
  (void)type;                                                     // Suppress unused parameter warning
  fprintf(stderr, "{ ");
  for (; first != last; ++first) {
    fprintf(stderr, "%d, ", *first);
  }
  fprintf(stderr, " }\n");
} /*}}}*/

}  // namespace base

#endif
