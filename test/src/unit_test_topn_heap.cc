// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>
#include <map>
#include <string>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/coding.h"
#include "base/random.h"
#include "base/status.h"
#include "base/topn_heap.h"

#include "test_base/include/test_base.h"

TEST(MaxTopNHeap, Test_Normal_Insert_Int_Data) { /*{{{*/
  using namespace base;

  MaxTopNHeap<int> max_topn_heap(5);

  int nums[] = {5, 7, 9, 1, 4, 6, 100, 3, 2, 8, 10, 15, 14, 13, 12, 14, -1, -100};
  int dest_nums[] = {100, 15, 14, 14, 13};
  std::vector<int> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = max_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<int> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_nums[i], topn_results[i]);
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MaxTopNHeap, Test_Normal_Insert_String_Data) { /*{{{*/
  using namespace base;

  MaxTopNHeap<std::string> max_topn_heap(6);

  std::string nums[] = {"0100", "1000", "0001", "0700", "0513", "0104", "3000", "9000", "4100", "1400", "7010", "4300"};
  std::vector<std::string> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = max_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::greater<std::string>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%s ", topn_results[i].c_str());
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MaxTopNHeap, Test_Normal_Insert_Double_Data) { /*{{{*/
  using namespace base;

  MaxTopNHeap<double> max_topn_heap(7);

  double nums[] = {112.23, 3.7, 15.9, 40.9, 432.1, -102.2, 77.5, 603.12, -1, -130.01, 83.2, 342.1, 99.2, -309.3};
  std::vector<double> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = max_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<double> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<double> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::greater<double>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%f ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MaxTopNHeap, Test_Normal_Insert_Rand_Int_Data) { /*{{{*/
  using namespace base;

  Code ret = InitRand();
  EXPECT_EQ(kOk, ret);

  MaxTopNHeap<uint32_t> max_topn_heap(20);

  uint32_t nums[1000];
  for (int i = 0; i < (int)(sizeof(nums) / sizeof(nums[0])); ++i) {
    nums[i] = rand() % 1000000;
  }
  std::vector<uint32_t> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  ret = max_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<uint32_t> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<uint32_t> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::greater<uint32_t>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MaxTopNHeap, Test_Normal_Insert_String_Data_Default_Len) { /*{{{*/
  using namespace base;

  MaxTopNHeap<std::string> max_topn_heap;

  std::string nums[] = {"0100", "1000", "0001", "0700", "0513", "0104", "3000",
                        "9000", "4100", "1400", "7010", "4300", "8001"};
  std::vector<std::string> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = max_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::greater<std::string>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%s ", topn_results[i].c_str());
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MaxTopNHeap, Test_Press_Insert_Rand_Int_OneMillion_Data) { /*{{{*/
  using namespace base;

  Code ret = InitRand();
  EXPECT_EQ(kOk, ret);

  MaxTopNHeap<uint32_t> max_topn_heap(20);

  for (int i = 0; i < 1000000; ++i) {
    uint32_t num = rand() % 1000000;

    ret = max_topn_heap.Insert(num);
    EXPECT_EQ(kOk, ret);
  }

  std::vector<uint32_t> topn_results;
  ret = max_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < (int)topn_results.size(); ++i) {
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Normal_Insert_Int_Data) { /*{{{*/
  using namespace base;

  MinTopNHeap<int> min_topn_heap(5);

  int nums[] = {5, 7, 9, 1, 4, 6, 100, 3, 2, 8, 10, 15, 14, 13, 12, 14, -1, -100};
  int dest_nums[] = {-100, -1, 1, 2, 3};
  std::vector<int> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = min_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<int> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_nums[i], topn_results[i]);
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Normal_Insert_String_Data) { /*{{{*/
  using namespace base;

  MinTopNHeap<std::string> min_topn_heap(6);

  std::string nums[] = {"0100", "1000", "0001", "0700", "0513", "0104", "3000", "9000", "4100", "1400", "7010", "4300"};
  std::vector<std::string> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = min_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::less<std::string>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%s ", topn_results[i].c_str());
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Normal_Insert_Double_Data) { /*{{{*/
  using namespace base;

  MinTopNHeap<double> min_topn_heap(7);

  double nums[] = {112.23, 3.7, 15.9, 40.9, 432.1, -102.2, 77.5, 603.12, -1, -130.01, 83.2, 342.1, 99.2, -309.3};
  std::vector<double> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = min_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<double> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<double> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::less<double>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%f ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Normal_Insert_Rand_Int_Data) { /*{{{*/
  using namespace base;

  Code ret = InitRand();
  EXPECT_EQ(kOk, ret);

  MinTopNHeap<uint32_t> min_topn_heap(20);

  uint32_t nums[1000];
  for (int i = 0; i < (int)(sizeof(nums) / sizeof(nums[0])); ++i) {
    nums[i] = rand() % 1000000;
  }
  std::vector<uint32_t> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  ret = min_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<uint32_t> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<uint32_t> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::less<uint32_t>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Normal_Insert_String_Data_Default_Len) { /*{{{*/
  using namespace base;

  MinTopNHeap<std::string> min_topn_heap;

  std::string nums[] = {"0100", "1000", "0001", "0700", "0513", "0104", "3000",
                        "9000", "4100", "1400", "7010", "4300", "8001"};
  std::vector<std::string> datas(nums, nums + sizeof(nums) / sizeof(nums[0]));

  Code ret = min_topn_heap.Insert(datas);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  std::vector<std::string> dest_datas(nums, nums + sizeof(nums) / sizeof(nums[0]));
  std::sort(dest_datas.begin(), dest_datas.end(), std::less<std::string>());
  for (int i = 0; i < (int)topn_results.size(); ++i) {
    EXPECT_EQ(dest_datas[i], topn_results[i]);
    fprintf(stderr, "%s ", topn_results[i].c_str());
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(MinTopNHeap, Test_Press_Insert_Rand_Int_OneMillion_Data) { /*{{{*/
  using namespace base;

  Code ret = InitRand();
  EXPECT_EQ(kOk, ret);

  MinTopNHeap<uint32_t> min_topn_heap(20);

  for (int i = 0; i < 1000000; ++i) {
    uint32_t num = rand() % 1000000;

    ret = min_topn_heap.Insert(num);
    EXPECT_EQ(kOk, ret);
  }

  std::vector<uint32_t> topn_results;
  ret = min_topn_heap.Pop(&topn_results);
  EXPECT_EQ(kOk, ret);

  for (int i = 0; i < (int)topn_results.size(); ++i) {
    fprintf(stderr, "%d ", topn_results[i]);
  }
  fprintf(stderr, "\n");
} /*}}}*/
