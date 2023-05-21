// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>

#include <stdint.h>
#include <stdio.h>

#include "base/bit_arr.h"
#include "base/common.h"
#include "base/status.h"
#include "base/util.h"

#include "test_base/include/test_base.h"

TEST(BitArr, Test_Normal_Put_OneBit) { /*{{{*/
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

TEST(BitArr, Test_Normal_Put_MultiBit) { /*{{{*/
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
