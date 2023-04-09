// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SORT_H_
#define BASE_SORT_H_

#include <stdio.h>

#include "base/common.h"
#include "base/status.h"

namespace base {

template <typename RandomIterator>
Code BinarySort(RandomIterator first, RandomIterator last, Compare comp) { /*{{{*/
  if (first > last) return kInvalidParam;
  if ((first == last) || (first + 1 == last)) return kOk;

  typename std::iterator_traits<RandomIterator>::value_type first_value = *first;
  RandomIterator tmp_first = first;
  RandomIterator tmp_last = last;
  --tmp_last;

  while (tmp_first < tmp_last) {
    while (comp(&(*tmp_last), &first_value) >= 0 && (tmp_first < tmp_last)) {
      --tmp_last;
    }
    if (tmp_first == tmp_last) break;
    *tmp_first = *tmp_last;

    while (comp(&(*tmp_first), &first_value) < 0 && (tmp_first < tmp_last)) {
      ++tmp_first;
    }
    if (tmp_first == tmp_last) break;
    *tmp_last = *tmp_first;
  }

  *tmp_first = first_value;

  Code ret = BinarySort(first, tmp_first, comp);
  if (ret != kOk) return ret;

  ret = BinarySort(tmp_first + 1, last, comp);
  if (ret != kOk) return ret;

  return kOk;
} /*}}}*/

}  // namespace base

#endif
