// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "base/coding.h"
#include "base/common.h"

#include "test_base/include/test_base.h"

template <typename C>
void TestSize(const C &container, size_t loop_size) {
  for (size_t i = 0; i < loop_size; ++i) {
    container.size();
  }
}

template <typename C, typename T>
void TestPushPop(C &container, const T &v, size_t loop_size, size_t container_size) { /*{{{*/
  for (size_t k = 0; k < loop_size; ++k) {
    for (size_t i = 0; i < container_size; ++i) {
      container.push_back(v);
    }

    for (size_t i = 0; i < container_size; ++i) {
      container.front();
    }

    for (size_t i = 0; i < container_size; ++i) {
      container.pop_front();
    }
  }
} /*}}}*/

TEST(String, Test_List_Press_Size_1000000) { /*{{{*/
  std::list<std::string> l;
  for (int i = 0; i < 1000; ++i) {
    l.push_back("aa");
  }

  TestSize(l, 1000000);
} /*}}}*/

TEST(String, Test_Deque_Press_Size_1000000) { /*{{{*/
  std::deque<std::string> l;
  for (int i = 0; i < 1000; ++i) {
    l.push_back("aa");
  }

  TestSize(l, 1000000);
} /*}}}*/

TEST(String, Test_Vector_Press_Size_1000000) { /*{{{*/
  std::vector<std::string> l;
  for (int i = 0; i < 1000; ++i) {
    l.push_back("aa");
  }

  TestSize(l, 1000000);
} /*}}}*/

TEST(String, Test_List_Press_PushPop_100000) { /*{{{*/
  std::list<std::string> l;
  TestPushPop(l, std::string("aa"), 100000, 100);
} /*}}}*/

TEST(String, Test_Deque_Press_PushPop_100000) { /*{{{*/
  std::deque<std::string> l;
  TestPushPop(l, std::string("aa"), 100000, 100);
} /*}}}*/
