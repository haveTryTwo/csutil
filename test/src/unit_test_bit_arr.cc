// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// 测试用例列表：
// ============================================================================
// 基础功能测试：
//   1. Test_Normal_Put_OneBit          - 测试BitArray（单个位）
//   2. Test_Normal_Put_MultiBit        - 测试多位BitArray
//
// Init()方法测试：
//   3. Test_Init_Normal                - 测试正常初始化
//   4. Test_Init_InvalidParam          - 测试初始化参数无效（bits_num=0）
//
// Put()方法测试：
//   5. Test_Put_Normal_True            - 测试正常设置true值
//   6. Test_Put_Normal_False           - 测试正常设置false值
//   7. Test_Put_InvalidParam_Index     - 测试Put索引越界
//   8. Test_Put_NotInit                - 测试未初始化时调用Put
//   9. Test_Put_Repeat                 - 测试重复设置同一个位
//
// Get()方法测试：
//  10. Test_Get_Normal_True            - 测试正常获取true值
//  11. Test_Get_Normal_False            - 测试正常获取false值
//  12. Test_Get_InvalidParam_Index     - 测试Get索引越界
//  13. Test_Get_InvalidParam_Null      - 测试Get参数为NULL
//  14. Test_Get_NotInit                 - 测试未初始化时调用Get
//  15. Test_Get_AllBits                 - 测试获取所有位的值
//
// Clear()方法测试：
//  16. Test_Clear_Normal                - 测试正常清空
//  17. Test_Clear_NotInit               - 测试未初始化时调用Clear
//  18. Test_Clear_AfterPut              - 测试设置后清空并验证ToString
//
// Size()方法测试：
//  19. Test_Size_Normal                 - 测试正常获取大小
//  20. Test_Size_InvalidParam          - 测试Size参数为NULL
//  21. Test_Size_NotInit                - 测试未初始化时调用Size
//
// ToString()方法测试：
//  22. Test_ToString_Normal_Empty      - 测试空数组的ToString
//  23. Test_ToString_Normal_Full       - 测试所有位都设置为1的ToString
//  24. Test_ToString_Normal_Partial     - 测试部分位设置的ToString
//  25. Test_ToString_InvalidParam      - 测试ToString参数为NULL
//  26. Test_ToString_NotInit            - 测试未初始化时调用ToString
//
// 边界值测试：
//  27. Test_Boundary_OneBit             - 测试1位数组
//  28. Test_Boundary_ByteBoundary       - 测试字节边界（8/16/24/32位）
//  29. Test_Boundary_LargeSize          - 测试大尺寸数组（1000位）
//
// 综合测试：
//  30. Test_Integration_AllOperations   - 测试所有操作的综合使用
// ============================================================================

#include <bitset>

#include <stdint.h>
#include <stdio.h>

#include "base/bit_arr.h"
#include "base/common.h"
#include "base/status.h"
#include "base/util.h"

#include "test_base/include/test_base.h"

TEST_D(BitArr, Test_Normal_Put_OneBit, "测试BitArray") { /*{{{*/
  using namespace base;

  for (uint32_t bit_len = 2; bit_len < 17; ++bit_len) {
    BitArr bit_arr(bit_len);
    Code ret = bit_arr.Init();
    EXPECT_EQ(kOk, ret);

    //        // BitSet Test
    //        std::bitset<4> bit_set;

    uint32_t index = 1;
    bool dst_value = true;
    ret = bit_arr.Put(index, dst_value);

    //        // BitSet Set
    //        bit_set.set(index, dst_value);

    bool tmp_value = false;
    ret = bit_arr.Get(index, &tmp_value);
    EXPECT_EQ(dst_value, tmp_value);

    std::string tmp_dst_str;
    for (int k = 0; k < (int)bit_len; ++k) {
      if (k == index) {
        tmp_dst_str.append(1, '1');
      } else {
        tmp_dst_str.append(1, '0');
      }
    }
    std::string dst_str;
    EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str));

    std::string tmp_str;
    ret = bit_arr.ToString(&tmp_str);
    EXPECT_EQ(kOk, ret);

    EXPECT_EQ(dst_str, tmp_str);
    fprintf(stderr, "bit_len:%u, %s, %s\n", bit_len, tmp_str.c_str(), dst_str.c_str());
  }
} /*}}}*/

TEST_D(BitArr, Test_Normal_Put_MultiBit, "测试多位BitArray") { /*{{{*/
  using namespace base;

  for (uint32_t bit_len = 2; bit_len < 17; ++bit_len) {
    BitArr bit_arr(bit_len);
    Code ret = bit_arr.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t index_arr[] = {1, 3, 5, 7, 9, 12, 13};
    for (int k = 0; k < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++k) {
      if (index_arr[k] >= bit_len) break;

      bool dst_value = true;
      ret = bit_arr.Put(index_arr[k], dst_value);

      bool tmp_value = false;
      ret = bit_arr.Get(index_arr[k], &tmp_value);
      EXPECT_EQ(dst_value, tmp_value);
    }

    std::string tmp_dst_str;
    for (int j = 0; j < (int)bit_len; ++j) {
      bool exist = false;
      for (int k = 0; k < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++k) {
        if (index_arr[k] >= bit_len) break;
        if (index_arr[k] > j) break;

        if (j == index_arr[k]) {
          exist = true;
          break;
        }
      }

      if (exist) {
        tmp_dst_str.append(1, '1');
      } else {
        tmp_dst_str.append(1, '0');
      }
    }
    std::string dst_str;
    EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str));

    std::string tmp_str;
    ret = bit_arr.ToString(&tmp_str);
    EXPECT_EQ(kOk, ret);

    EXPECT_EQ(dst_str, tmp_str);
    fprintf(stderr, "bit_len:%u, %s, %s\n", bit_len, tmp_str.c_str(), dst_str.c_str());
  }
} /*}}}*/

TEST_D(BitArr, Test_Init_Normal, "测试正常初始化") { /*{{{*/
  using namespace base;

  uint32_t bit_len_arr[] = {1, 8, 16, 32, 100};
  for (int i = 0; i < (int)(sizeof(bit_len_arr) / sizeof(bit_len_arr[0])); ++i) {
    BitArr bit_arr(bit_len_arr[i]);
    Code ret = bit_arr.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t size = 0;
    ret = bit_arr.Size(&size);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bit_len_arr[i], size);
  }
} /*}}}*/

TEST_D(BitArr, Test_Init_InvalidParam, "测试初始化参数无效") { /*{{{*/
  using namespace base;

  BitArr bit_arr(0);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_Put_Normal_True, "测试正常设置true值") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 测试边界位置：第一个位和最后一个位
  uint32_t index_arr[] = {0, bit_len - 1};
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    uint32_t index = index_arr[i];
    bool dst_value = true;
    ret = bit_arr.Put(index, dst_value);
    EXPECT_EQ(kOk, ret);

    bool tmp_value = false;
    ret = bit_arr.Get(index, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(dst_value, tmp_value);
  }
} /*}}}*/

TEST_D(BitArr, Test_Put_Normal_False, "测试正常设置false值") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 先设置为true
  uint32_t index = 5;
  ret = bit_arr.Put(index, true);
  EXPECT_EQ(kOk, ret);

  bool tmp_value = false;
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, tmp_value);

  // 再设置为false
  ret = bit_arr.Put(index, false);
  EXPECT_EQ(kOk, ret);

  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, tmp_value);
} /*}}}*/

TEST_D(BitArr, Test_Put_InvalidParam_Index, "测试Put索引越界") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 测试越界索引
  uint32_t invalid_index = bit_len;
  ret = bit_arr.Put(invalid_index, true);
  EXPECT_EQ(kInvalidParam, ret);

  invalid_index = bit_len + 1;
  ret = bit_arr.Put(invalid_index, true);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_Put_NotInit, "测试未初始化时调用Put") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  // 不调用Init()

  Code ret = bit_arr.Put(0, true);
  EXPECT_EQ(kNotInit, ret);
} /*}}}*/

TEST_D(BitArr, Test_Put_Repeat, "测试重复设置同一个位") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  uint32_t index = 7;

  // 先设置为true
  ret = bit_arr.Put(index, true);
  EXPECT_EQ(kOk, ret);
  bool tmp_value = false;
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, tmp_value);

  // 再设置为false
  ret = bit_arr.Put(index, false);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, tmp_value);

  // 再次设置为true
  ret = bit_arr.Put(index, true);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, tmp_value);
} /*}}}*/

TEST_D(BitArr, Test_Get_Normal_True, "测试正常获取true值") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  uint32_t index = 10;
  ret = bit_arr.Put(index, true);
  EXPECT_EQ(kOk, ret);

  bool tmp_value = false;
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, tmp_value);
} /*}}}*/

TEST_D(BitArr, Test_Get_Normal_False, "测试正常获取false值") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 不设置任何位，默认应该都是false
  uint32_t index = 5;
  bool tmp_value = true;  // 初始化为true，验证会被设置为false
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, tmp_value);
} /*}}}*/

TEST_D(BitArr, Test_Get_InvalidParam_Index, "测试Get索引越界") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  bool tmp_value = false;
  uint32_t invalid_index = bit_len;
  ret = bit_arr.Get(invalid_index, &tmp_value);
  EXPECT_EQ(kInvalidParam, ret);

  invalid_index = bit_len + 1;
  ret = bit_arr.Get(invalid_index, &tmp_value);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_Get_InvalidParam_Null, "测试Get参数为NULL") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  ret = bit_arr.Get(0, NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_Get_NotInit, "测试未初始化时调用Get") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  // 不调用Init()

  bool tmp_value = false;
  Code ret = bit_arr.Get(0, &tmp_value);
  EXPECT_EQ(kNotInit, ret);
} /*}}}*/

TEST_D(BitArr, Test_Get_AllBits, "测试获取所有位的值") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 验证初始状态所有位都是false
  for (uint32_t i = 0; i < bit_len; ++i) {
    bool tmp_value = true;  // 初始化为true，验证会被设置为false
    ret = bit_arr.Get(i, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, tmp_value);
  }
} /*}}}*/

TEST_D(BitArr, Test_Clear_Normal, "测试正常清空") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 设置一些位
  uint32_t index_arr[] = {1, 3, 5, 7, 9, 11};
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    ret = bit_arr.Put(index_arr[i], true);
    EXPECT_EQ(kOk, ret);
  }

  // 清空
  ret = bit_arr.Clear();
  EXPECT_EQ(kOk, ret);

  // 验证所有位都是false
  for (uint32_t i = 0; i < bit_len; ++i) {
    bool tmp_value = true;
    ret = bit_arr.Get(i, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, tmp_value);
  }
} /*}}}*/

TEST_D(BitArr, Test_Clear_NotInit, "测试未初始化时调用Clear") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  // 不调用Init()

  Code ret = bit_arr.Clear();
  EXPECT_EQ(kNotInit, ret);
} /*}}}*/

TEST_D(BitArr, Test_Clear_AfterPut, "测试设置后清空并验证ToString") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 8;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 设置一些位
  ret = bit_arr.Put(1, true);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Put(3, true);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Put(5, true);
  EXPECT_EQ(kOk, ret);

  // 清空
  ret = bit_arr.Clear();
  EXPECT_EQ(kOk, ret);

  // 验证ToString结果全为0
  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);

  std::string expected_str(bit_len, '0');
  EXPECT_EQ(expected_str, tmp_str);
} /*}}}*/

TEST_D(BitArr, Test_Size_Normal, "测试正常获取大小") { /*{{{*/
  using namespace base;

  uint32_t bit_len_arr[] = {1, 8, 16, 32, 100, 256};
  for (int i = 0; i < (int)(sizeof(bit_len_arr) / sizeof(bit_len_arr[0])); ++i) {
    BitArr bit_arr(bit_len_arr[i]);
    Code ret = bit_arr.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t size = 0;
    ret = bit_arr.Size(&size);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bit_len_arr[i], size);
  }
} /*}}}*/

TEST_D(BitArr, Test_Size_InvalidParam, "测试Size参数为NULL") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  ret = bit_arr.Size(NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_Size_NotInit, "测试未初始化时调用Size") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  // 不调用Init()

  uint32_t size = 0;
  Code ret = bit_arr.Size(&size);
  EXPECT_EQ(kNotInit, ret);
} /*}}}*/

TEST_D(BitArr, Test_ToString_Normal_Empty, "测试空数组的ToString") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 8;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);

  std::string expected_str(bit_len, '0');
  EXPECT_EQ(expected_str, tmp_str);
} /*}}}*/

TEST_D(BitArr, Test_ToString_Normal_Full, "测试所有位都设置为1的ToString") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 8;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 设置所有位为1
  for (uint32_t i = 0; i < bit_len; ++i) {
    ret = bit_arr.Put(i, true);
    EXPECT_EQ(kOk, ret);
  }

  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);

  std::string expected_str(bit_len, '1');
  EXPECT_EQ(expected_str, tmp_str);
} /*}}}*/

TEST_D(BitArr, Test_ToString_Normal_Partial, "测试部分位设置的ToString") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 8;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 设置特定位置
  uint32_t index_arr[] = {0, 2, 4, 6};
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    ret = bit_arr.Put(index_arr[i], true);
    EXPECT_EQ(kOk, ret);
  }

  // 构建期望字符串
  std::string tmp_dst_str;
  for (int j = 0; j < (int)bit_len; ++j) {
    bool exist = false;
    for (int k = 0; k < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++k) {
      if (j == index_arr[k]) {
        exist = true;
        break;
      }
    }

    if (exist) {
      tmp_dst_str.append(1, '1');
    } else {
      tmp_dst_str.append(1, '0');
    }
  }
  std::string dst_str;
  EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str));

  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);

  EXPECT_EQ(dst_str, tmp_str);
} /*}}}*/

TEST_D(BitArr, Test_ToString_InvalidParam, "测试ToString参数为NULL") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  ret = bit_arr.ToString(NULL);
  EXPECT_EQ(kInvalidParam, ret);
} /*}}}*/

TEST_D(BitArr, Test_ToString_NotInit, "测试未初始化时调用ToString") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);
  // 不调用Init()

  std::string tmp_str;
  Code ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kNotInit, ret);
} /*}}}*/

TEST_D(BitArr, Test_Boundary_OneBit, "测试1位数组") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 1;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 测试Size
  uint32_t size = 0;
  ret = bit_arr.Size(&size);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, size);

  // 测试Put和Get
  uint32_t index = 0;
  ret = bit_arr.Put(index, true);
  EXPECT_EQ(kOk, ret);

  bool tmp_value = false;
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(true, tmp_value);

  // 测试ToString - 使用与现有测试用例相同的方式
  std::string tmp_dst_str;
  for (int k = 0; k < (int)bit_len; ++k) {
    if (k == index) {
      tmp_dst_str.append(1, '1');
    } else {
      tmp_dst_str.append(1, '0');
    }
  }
  std::string dst_str;
  EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str));

  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(dst_str, tmp_str);

  // 设置为false
  ret = bit_arr.Put(index, false);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Get(index, &tmp_value);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(false, tmp_value);

  // 再次测试ToString
  tmp_dst_str.clear();
  for (int k = 0; k < (int)bit_len; ++k) {
    tmp_dst_str.append(1, '0');
  }
  std::string dst_str2;
  EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str2));

  tmp_str.clear();
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(dst_str2, tmp_str);
} /*}}}*/

TEST_D(BitArr, Test_Boundary_ByteBoundary, "测试字节边界") { /*{{{*/
  using namespace base;

  uint32_t bit_len_arr[] = {8, 16, 24, 32};
  for (int i = 0; i < (int)(sizeof(bit_len_arr) / sizeof(bit_len_arr[0])); ++i) {
    uint32_t bit_len = bit_len_arr[i];
    BitArr bit_arr(bit_len);
    Code ret = bit_arr.Init();
    EXPECT_EQ(kOk, ret);

    // 测试边界位置：最后一个字节的第一个位和最后一个位
    uint32_t last_byte_start = (bit_len / 8 - 1) * 8;
    uint32_t last_byte_end = bit_len - 1;

    // 设置边界位
    ret = bit_arr.Put(last_byte_start, true);
    EXPECT_EQ(kOk, ret);
    ret = bit_arr.Put(last_byte_end, true);
    EXPECT_EQ(kOk, ret);

    // 验证
    bool tmp_value = false;
    ret = bit_arr.Get(last_byte_start, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, tmp_value);

    ret = bit_arr.Get(last_byte_end, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, tmp_value);

    // 验证Size
    uint32_t size = 0;
    ret = bit_arr.Size(&size);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bit_len, size);
  }
} /*}}}*/

TEST_D(BitArr, Test_Boundary_LargeSize, "测试大尺寸数组") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 1000;
  BitArr bit_arr(bit_len);
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 测试Size
  uint32_t size = 0;
  ret = bit_arr.Size(&size);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, size);

  // 测试边界位置
  uint32_t index_arr[] = {0, 100, 500, 999};
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    uint32_t index = index_arr[i];
    ret = bit_arr.Put(index, true);
    EXPECT_EQ(kOk, ret);

    bool tmp_value = false;
    ret = bit_arr.Get(index, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, tmp_value);
  }

  // 测试ToString
  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, tmp_str.size());
} /*}}}*/

TEST_D(BitArr, Test_Integration_AllOperations, "测试所有操作的综合使用") { /*{{{*/
  using namespace base;

  uint32_t bit_len = 16;
  BitArr bit_arr(bit_len);

  // 1. Init
  Code ret = bit_arr.Init();
  EXPECT_EQ(kOk, ret);

  // 2. Size
  uint32_t size = 0;
  ret = bit_arr.Size(&size);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, size);

  // 3. Put多个位
  uint32_t index_arr[] = {1, 3, 5, 7, 9};
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    ret = bit_arr.Put(index_arr[i], true);
    EXPECT_EQ(kOk, ret);
  }

  // 4. Get验证
  for (int i = 0; i < (int)(sizeof(index_arr) / sizeof(index_arr[0])); ++i) {
    bool tmp_value = false;
    ret = bit_arr.Get(index_arr[i], &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, tmp_value);
  }

  // 5. ToString
  std::string tmp_str;
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, tmp_str.size());

  // 6. Clear
  ret = bit_arr.Clear();
  EXPECT_EQ(kOk, ret);

  // 7. 验证Clear后所有位为0
  for (uint32_t i = 0; i < bit_len; ++i) {
    bool tmp_value = true;
    ret = bit_arr.Get(i, &tmp_value);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, tmp_value);
  }

  // 8. Clear后再次Put
  ret = bit_arr.Put(0, true);
  EXPECT_EQ(kOk, ret);
  ret = bit_arr.Put(bit_len - 1, true);
  EXPECT_EQ(kOk, ret);

  // 9. 再次ToString - 验证内容
  uint32_t new_index_arr[] = {0, bit_len - 1};
  std::string tmp_dst_str;
  for (int j = 0; j < (int)bit_len; ++j) {
    bool exist = false;
    for (int k = 0; k < (int)(sizeof(new_index_arr) / sizeof(new_index_arr[0])); ++k) {
      if (j == new_index_arr[k]) {
        exist = true;
        break;
      }
    }

    if (exist) {
      tmp_dst_str.append(1, '1');
    } else {
      tmp_dst_str.append(1, '0');
    }
  }
  std::string dst_str;
  EXPECT_EQ(base::kOk, base::Reverse(tmp_dst_str, &dst_str));

  tmp_str.clear();
  ret = bit_arr.ToString(&tmp_str);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, tmp_str.size());
  EXPECT_EQ(dst_str, tmp_str);

  // 10. 再次Size
  ret = bit_arr.Size(&size);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(bit_len, size);
} /*}}}*/
