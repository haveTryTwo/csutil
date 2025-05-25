// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "base/bkd_tree.h"
#include "base/common.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(BkdTree, Test_Normal_bkdtree) { /*{{{*/
  using namespace base;
  int k = 2;
  BKDTree tree(k);

  std::vector<BkdTreePoint> points = {{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}};
  for (auto& pt : points) {
    tree.insert(pt);
  }

  BkdTreePoint low(k), high(k);
  low.coords = {3, 2};
  high.coords = {10, 7};

  std::vector<BkdTreePoint> result;
  tree.rangeQuery(low, high, result);

  std::cout << "Range query result:" << std::endl;
  for (auto& pt : result) {
    for (int v : pt.coords) std::cout << v << " ";
    std::cout << std::endl;
  }
} /*}}}*/
