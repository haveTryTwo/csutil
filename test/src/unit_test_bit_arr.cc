// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <bitset>

#include <stdio.h>
#include <stdint.h>

#include "base/common.h"
#include "base/status.h"
#include "base/bit_arr.h"

#include "test_base/include/test_base.h"

TEST(BitArr, Test_Normal_Put)
{/*{{{*/
    using namespace base;

    for (uint32_t bit_len = 2; bit_len < 17; ++bit_len)
    {
        BitArr bit_arr(bit_len);
        Code ret = bit_arr.Init();
        EXPECT_EQ(kOk, ret);

//        // BitSet Test
//        std::bitset<4> bit_set;

        uint32_t pos = 1;
        bool dst_value = true;
        ret = bit_arr.Put(pos, dst_value);

//        // BitSet Set
//        bit_set.set(pos, dst_value);

        bool tmp_value = false;
        ret = bit_arr.Get(pos, &tmp_value);
        EXPECT_EQ(dst_value, tmp_value);

        std::string dst_str = "0000000010";
        std::string tmp_str;
        ret = bit_arr.ToString(&tmp_str);
        EXPECT_EQ(kOk, ret);

//        std::string bit_str = bit_set.to_string();
//        EXPECT_EQ(tmp_str, bit_str);
//        EXPECT_EQ(dst_str, tmp_str);
        fprintf(stderr, "bit_len:%u, %s\n", bit_len, tmp_str.c_str());
    }

}/*}}}*/
