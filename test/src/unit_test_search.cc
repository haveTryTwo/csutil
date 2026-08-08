// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include "base/search.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

namespace {

TEST_D(Search, CompareNum_Int32, "测试目标:CompareNum比较int32类型") { /*{{{*/
  using namespace base;

  int32_t a = 10;
  int32_t b = 20;
  int32_t c = 10;

  EXPECT_EQ(-1, CompareNum<int32_t>(&a, &b));
  EXPECT_EQ(1, CompareNum<int32_t>(&b, &a));
  EXPECT_EQ(0, CompareNum<int32_t>(&a, &c));
} /*}}}*/

TEST_D(Search, CompareNum_UInt32, "测试目标:CompareNum比较uint32_t类型") { /*{{{*/
  using namespace base;

  uint32_t a = 100;
  uint32_t b = 200;
  uint32_t c = 100;

  EXPECT_EQ(-1, CompareNum<uint32_t>(&a, &b));
  EXPECT_EQ(1, CompareNum<uint32_t>(&b, &a));
  EXPECT_EQ(0, CompareNum<uint32_t>(&a, &c));
} /*}}}*/

TEST_D(Search, CompareNum_UInt64, "测试目标:CompareNum比较uint64_t类型") { /*{{{*/
  using namespace base;

  uint64_t a = 1000000000ULL;
  uint64_t b = 2000000000ULL;
  uint64_t c = 1000000000ULL;

  EXPECT_EQ(-1, CompareNum<uint64_t>(&a, &b));
  EXPECT_EQ(1, CompareNum<uint64_t>(&b, &a));
  EXPECT_EQ(0, CompareNum<uint64_t>(&a, &c));
} /*}}}*/

TEST_D(Search, CompareNum_Double, "测试目标:CompareNum比较double类型") { /*{{{*/
  using namespace base;

  double a = 10.5;
  double b = 20.5;
  double c = 10.5;

  EXPECT_EQ(-1, CompareNum<double>(&a, &b));
  EXPECT_EQ(1, CompareNum<double>(&b, &a));
  EXPECT_EQ(0, CompareNum<double>(&a, &c));
} /*}}}*/

TEST_D(Search, ComparePair_StringPair, "测试目标:ComparePair比较string pair类型") { /*{{{*/
  using namespace base;
  using StringPair = std::pair<std::string, std::string>;

  StringPair a = {"apple", "red"};
  StringPair b = {"banana", "yellow"};
  StringPair c = {"apple", "green"};

  EXPECT_EQ(-1, ComparePair<StringPair>(&a, &b));
  EXPECT_EQ(1, ComparePair<StringPair>(&b, &a));
  EXPECT_EQ(0, ComparePair<StringPair>(&a, &c));  // 只比较first
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Array_Basic, "测试目标:降序数组二分查找基本功能") { /*{{{*/
  using namespace base;

  // 降序数组: {100, 80, 60, 40, 20}
  uint32_t arr[] = {100, 80, 60, 40, 20};
  int size = 5;
  int pos = -1;

  // 查找存在的值
  uint32_t target = 60;
  Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(2, pos);

  // 查找范围内的值 (应返回对应范围的起始位置)
  target = 65;  // 在[60, 80)范围内, 应返回60的位置
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(2, pos);

  // 查找大于最大值的值
  target = 150;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  // 查找小于最小值的值
  target = 10;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(4, pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Array_SingleElement, "测试目标:降序数组二分查找单元素") { /*{{{*/
  using namespace base;

  uint32_t arr[] = {50};
  int size = 1;
  int pos = -1;

  uint32_t target = 50;
  Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  target = 60;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  target = 40;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Array_TwoElements, "测试目标:降序数组二分查找两元素") { /*{{{*/
  using namespace base;

  uint32_t arr[] = {100, 50};
  int size = 2;
  int pos = -1;

  uint32_t target = 100;
  Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  target = 75;  // 在[50, 100)范围内
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(1, pos);

  target = 50;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(1, pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Array_InvalidParams, "测试目标:降序数组二分查找参数校验") { /*{{{*/
  using namespace base;

  uint32_t arr[] = {100, 80, 60, 40, 20};
  int size = 5;
  int pos = -1;
  uint32_t target = 60;

  // NULL array
  Code ret = BinaryDescendSearch((uint32_t*)NULL, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // Invalid size
  ret = BinaryDescendSearch(arr, 0, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  ret = BinaryDescendSearch(arr, -1, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // NULL pos
  ret = BinaryDescendSearch(arr, size, target, (int*)NULL, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Array_Basic, "测试目标:升序数组二分查找基本功能") { /*{{{*/
  using namespace base;

  // 升序数组: {20, 40, 60, 80, 100}
  uint32_t arr[] = {20, 40, 60, 80, 100};
  int size = 5;
  int pos = -1;

  // 查找存在的值
  uint32_t target = 60;
  Code ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(2, pos);

  // 查找范围内的值 (应返回对应范围的起始位置)
  target = 55;  // 在[40, 60)范围内, 应返回40的位置
  ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(1, pos);

  // 查找小于最小值的值
  target = 10;
  ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  // 查找大于最大值的值
  target = 150;
  ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(4, pos);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Array_SingleElement, "测试目标:升序数组二分查找单元素") { /*{{{*/
  using namespace base;

  uint32_t arr[] = {50};
  int size = 1;
  int pos = -1;

  uint32_t target = 50;
  Code ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  target = 40;
  ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  target = 60;
  ret = BinaryAscendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Array_InvalidParams, "测试目标:升序数组二分查找参数校验") { /*{{{*/
  using namespace base;

  uint32_t arr[] = {20, 40, 60, 80, 100};
  int size = 5;
  int pos = -1;
  uint32_t target = 60;

  // NULL array
  Code ret = BinaryAscendSearch((uint32_t*)NULL, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // Invalid size
  ret = BinaryAscendSearch(arr, 0, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  ret = BinaryAscendSearch(arr, -1, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // NULL pos
  ret = BinaryAscendSearch(arr, size, target, (int*)NULL, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Iterator_Basic, "测试目标:降序迭代器二分查找基本功能") { /*{{{*/
  using namespace base;

  // 降序vector: {100, 80, 60, 40, 20}
  std::vector<uint32_t> vec = {100, 80, 60, 40, 20};
  std::vector<uint32_t>::iterator pos;

  // 查找存在的值
  uint32_t target = 60;
  Code ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(60U, *pos);
  EXPECT_EQ(2, pos - vec.begin());

  // 查找范围内的值
  target = 65;  // 在[60, 80)范围内
  ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(60U, *pos);

  // 查找大于最大值的值
  target = 150;
  ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(100U, *pos);

  // 查找小于最小值的值
  target = 10;
  ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(20U, *pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Iterator_EmptyRange, "测试目标:降序迭代器二分查找空范围") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec;
  std::vector<uint32_t>::iterator pos;
  uint32_t target = 50;

  Code ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(vec.begin(), pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Iterator_SingleElement, "测试目标:降序迭代器二分查找单元素") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec = {50};
  std::vector<uint32_t>::iterator pos;

  uint32_t target = 50;
  Code ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(50U, *pos);

  target = 60;
  ret = BinaryDescendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(50U, *pos);
} /*}}}*/

TEST_D(Search, BinaryDescendSearch_Iterator_InvalidParams, "测试目标:降序迭代器二分查找参数校验") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec = {100, 80, 60, 40, 20};
  std::vector<uint32_t>::iterator pos;
  uint32_t target = 60;

  // first > last (invalid range)
  Code ret = BinaryDescendSearch(vec.end(), vec.begin(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // NULL pos
  ret =
      BinaryDescendSearch(vec.begin(), vec.end(), target, (std::vector<uint32_t>::iterator*)NULL, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Iterator_Basic, "测试目标:升序迭代器二分查找基本功能") { /*{{{*/
  using namespace base;

  // 升序vector: {20, 40, 60, 80, 100}
  std::vector<uint32_t> vec = {20, 40, 60, 80, 100};
  std::vector<uint32_t>::iterator pos;

  // 查找存在的值
  uint32_t target = 60;
  Code ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(60U, *pos);
  EXPECT_EQ(2, pos - vec.begin());

  // 查找范围内的值
  target = 55;  // 在[40, 60)范围内
  ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(40U, *pos);

  // 查找小于最小值的值
  target = 10;
  ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(20U, *pos);

  // 查找大于最大值的值
  target = 150;
  ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(100U, *pos);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Iterator_EmptyRange, "测试目标:升序迭代器二分查找空范围") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec;
  std::vector<uint32_t>::iterator pos;
  uint32_t target = 50;

  Code ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(vec.begin(), pos);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Iterator_SingleElement, "测试目标:升序迭代器二分查找单元素") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec = {50};
  std::vector<uint32_t>::iterator pos;

  uint32_t target = 50;
  Code ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(50U, *pos);

  target = 40;
  ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(50U, *pos);
} /*}}}*/

TEST_D(Search, BinaryAscendSearch_Iterator_InvalidParams, "测试目标:升序迭代器二分查找参数校验") { /*{{{*/
  using namespace base;

  std::vector<uint32_t> vec = {20, 40, 60, 80, 100};
  std::vector<uint32_t>::iterator pos;
  uint32_t target = 60;

  // first > last (invalid range)
  Code ret = BinaryAscendSearch(vec.end(), vec.begin(), target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);

  // NULL pos
  ret =
      BinaryAscendSearch(vec.begin(), vec.end(), target, (std::vector<uint32_t>::iterator*)NULL, CompareNum<uint32_t>);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(Search, BinarySearch_LargeArray, "测试目标:大数组二分查找性能") { /*{{{*/
  using namespace base;

  // 创建大的升序数组
  const int size = 100000;
  std::vector<uint32_t> vec;
  for (int i = 0; i < size; ++i) {
    vec.push_back(i * 10);  // 0, 10, 20, 30, ...
  }

  std::vector<uint32_t>::iterator pos;

  auto start_time = std::chrono::high_resolution_clock::now();

  // 执行多次查找
  const int iterations = 10000;
  for (int i = 0; i < iterations; ++i) {
    uint32_t target = (i % size) * 10;
    Code ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint32_t>);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(target, *pos);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  printf("大数组二分查找性能测试: 数组大小=%d, 查找次数=%d, 总耗时=%lld μs, 平均每次=%.2f μs\n", size, iterations,
         (long long)duration.count(), (double)duration.count() / iterations);
} /*}}}*/

TEST_D(Search, BinarySearch_DifferentTypes, "测试目标:不同类型数据的二分查找") { /*{{{*/
  using namespace base;

  // 测试int64_t类型
  {
    int64_t arr[] = {1000000000LL, 500000000LL, 100000000LL, 50000000LL, 10000000LL};
    int size = 5;
    int pos = -1;
    int64_t target = 100000000LL;

    Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<int64_t>);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(2, pos);
  }

  // 测试double类型
  {
    double arr[] = {100.5, 80.3, 60.1, 40.9, 20.7};
    int size = 5;
    int pos = -1;
    double target = 60.1;

    Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<double>);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(2, pos);
  }

  // 测试uint64_t类型
  {
    std::vector<uint64_t> vec = {10ULL, 20ULL, 30ULL, 40ULL, 50ULL};
    std::vector<uint64_t>::iterator pos;
    uint64_t target = 30ULL;

    Code ret = BinaryAscendSearch(vec.begin(), vec.end(), target, &pos, CompareNum<uint64_t>);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(30ULL, *pos);
  }
} /*}}}*/

TEST_D(Search, BinarySearch_BoundaryValues, "测试目标:边界值测试") { /*{{{*/
  using namespace base;

  // 测试最大值和最小值
  uint32_t arr[] = {UINT32_MAX, UINT32_MAX / 2, 100, 50, 0};
  int size = 5;
  int pos = -1;

  // 查找最大值
  uint32_t target = UINT32_MAX;
  Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(0, pos);

  // 查找最小值
  target = 0;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(4, pos);

  // 查找中间值
  target = UINT32_MAX / 2;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(1, pos);
} /*}}}*/

TEST_D(Search, BinarySearch_DuplicateValues, "测试目标:重复值测试") { /*{{{*/
  using namespace base;

  // 降序数组包含重复值
  uint32_t arr[] = {100, 80, 80, 80, 60, 40, 40, 20};
  int size = 8;
  int pos = -1;

  // 查找重复的80
  uint32_t target = 80;
  Code ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  // 应该找到其中一个80的位置 (位置1, 2, 或3)
  EXPECT_TRUE(pos >= 1 && pos <= 3);
  EXPECT_EQ(80U, arr[pos]);

  // 查找重复的40
  target = 40;
  ret = BinaryDescendSearch(arr, size, target, &pos, CompareNum<uint32_t>);
  EXPECT_EQ(kOk, ret);
  EXPECT_TRUE(pos >= 5 && pos <= 6);
  EXPECT_EQ(40U, arr[pos]);
} /*}}}*/

TEST_D(Search, BinarySearch_RangeSearch, "测试目标:范围查找功能测试") { /*{{{*/
  using namespace base;

  // 降序数组模拟索引: {100, 80, 60, 40, 20}
  // 每个值代表一个范围的起始位置
  // 100: [100, ∞), 80: [80, 100), 60: [60, 80), 40: [40, 60), 20: [20, 40)
  uint32_t arr[] = {100, 80, 60, 40, 20};
  int size = 5;
  int pos = -1;

  // 测试各个范围内的查找
  struct TestCase {
    uint32_t value;
    int expected_pos;
    const char* description;
  };

  TestCase test_cases[] = {{150, 0, "大于最大值"},     {100, 0, "等于最大值"},     {95, 1, "在[80,100)范围内"},
                           {80, 1, "等于80"},          {75, 2, "在[60,80)范围内"}, {60, 2, "等于60"},
                           {55, 3, "在[40,60)范围内"}, {40, 3, "等于40"},          {35, 4, "在[20,40)范围内"},
                           {20, 4, "等于20"},          {15, 4, "小于最小值"}};

  for (const auto& tc : test_cases) {
    Code ret = BinaryDescendSearch(arr, size, tc.value, &pos, CompareNum<uint32_t>);
    EXPECT_EQ(kOk, ret);
    if (pos != tc.expected_pos) {
      printf("测试失败: %s, 查找值=%u, 期望位置=%d, 实际位置=%d\n", tc.description, tc.value, tc.expected_pos, pos);
    }
    EXPECT_EQ(tc.expected_pos, pos);
  }
} /*}}}*/

TEST_D(Search, Print_Function, "测试目标:Print函数调试输出") { /*{{{*/
  using namespace base;

  std::vector<int> vec = {1, 2, 3, 4, 5};

  printf("\n测试Print函数输出: \n");
  Print(vec.begin(), vec.end(), 0);

  // Print函数主要用于调试，这里只是确保它能正常运行不崩溃
  EXPECT_TRUE(true);
} /*}}}*/

}  // namespace
