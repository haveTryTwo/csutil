// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/common.h"
#include "base/sort.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

template <typename T>
int CompareIntAscend(const void *first, const void *second) {
  return (*(T *)first - *(T *)second);
}

template <typename T>
int CompareIntDescend(const void *first, const void *second) {
  return (*(T *)second - *(T *)first);
}

TEST(BinarySort, Test_Normal_Ascend_Sort_Int) { /*{{{*/
  using namespace base;

  int arr[] = {-1, 10, 8, 6, 10, 17, 4, 3, 9, 10, 20, 50, -5, 20};
  Code ret = BinarySort(arr, arr + sizeof(arr) / sizeof(arr[0]), CompareIntAscend<int>);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
    fprintf(stderr, "%d ", arr[i]);
  }
  fprintf(stderr, "\n");

  int qsort_arr[] = {-1, 10, 8, 6, 10, 17, 4, 3, 9, 10, 20, 50, -5, 20};
  qsort(qsort_arr, sizeof(qsort_arr) / sizeof(qsort_arr[0]), sizeof(qsort_arr[0]), CompareIntAscend<int>);
  for (int i = 0; i < sizeof(qsort_arr) / sizeof(qsort_arr[0]); ++i) {
    fprintf(stderr, "%d ", qsort_arr[i]);
  }
  fprintf(stderr, "\n");

  // Check BinarySort and qsort
  for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
    EXPECT_EQ(arr[i], qsort_arr[i]);
  }
} /*}}}*/

TEST(BinarySort, Test_Normal_Descend_Sort_Int) { /*{{{*/
  using namespace base;

  int arr[] = {-1, 10, 8, 6, 10, 17, 4, 3, 9, 10, 20, 50, -5, 20};
  Code ret = BinarySort(arr, arr + sizeof(arr) / sizeof(arr[0]), CompareIntDescend<int>);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
    fprintf(stderr, "%d ", arr[i]);
  }
  fprintf(stderr, "\n");

  int qsort_arr[] = {-1, 10, 8, 6, 10, 17, 4, 3, 9, 10, 20, 50, -5, 20};
  qsort(qsort_arr, sizeof(qsort_arr) / sizeof(qsort_arr[0]), sizeof(qsort_arr[0]), CompareIntDescend<int>);
  for (int i = 0; i < sizeof(qsort_arr) / sizeof(qsort_arr[0]); ++i) {
    fprintf(stderr, "%d ", qsort_arr[i]);
  }
  fprintf(stderr, "\n");

  // Check BinarySort and qsort
  for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i) {
    EXPECT_EQ(arr[i], qsort_arr[i]);
  }
} /*}}}*/
