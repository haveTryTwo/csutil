// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SEARCH_H_
#define BASE_SEARCH_H_

#include <stdio.h>

#include "base/status.h"

namespace base
{

typedef int (*Compare)(const void *first, const void *second);

template <typename T>
int CompareNum(const void *first, const void *second)
{
    return (*(T*)first - *(T*)second);
}

template <typename T>
Code BinarySearch(const T *arr, int size, const T &data, int *pos, Compare comp)
{
    if (size <= 0 || pos == NULL) return kInvalidParam;

    int low = 0;
    int mid = 0;
    int high = size - 1;

    while (low < high)
    {
        mid = (low+high)/2;
        if (comp(arr+mid, &data) > 0)
            low = mid+1;
        else if (comp(arr+mid, &data) < 0)
            high = mid;
        else
        {
            low = mid;
            break;
        }
    }

    *pos = low;
    return kOk;
}

}

#endif
