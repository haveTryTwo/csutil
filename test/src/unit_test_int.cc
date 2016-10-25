// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "base/int.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(GetInt64, ZeroData)
{/*{{{*/
    using namespace base;

    uint64_t zero_data = 0;

    std::string str_data = "0";
    int64_t data = 0;
    base::Code ret = GetInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(zero_data, data);
}/*}}}*/

TEST(GetInt64, SmallData)
{/*{{{*/
    using namespace base;

    uint64_t small_data = 1234567;

    std::string str_data = "1234567";
    int64_t data = 0;
    base::Code ret = GetInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(small_data, data);
}/*}}}*/

TEST(GetInt64, BigData)
{/*{{{*/
    using namespace base;

//    uint64_t big_data = 18446744073709551616;
    uint64_t big_data = 0xffffffffffffffff;
    fprintf(stdout, "%#llx, %llu\n", big_data, big_data);

    std::string str_data = "18446744073709551615";
//    str_data = "9223372036854775808";
    int64_t data = 0;
    base::Code ret = GetInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(big_data, data);
    fprintf(stdout, "%#llx, %llu\n", data, data);
}/*}}}*/

TEST(GetUInt64, ZeroData)
{/*{{{*/
    using namespace base;

    uint64_t zero_data = 0;

    std::string str_data = "0";
    uint64_t data = 0;
    base::Code ret = GetUInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(zero_data, data);
}/*}}}*/

TEST(GetUInt64, SmallData)
{/*{{{*/
    using namespace base;

    uint64_t small_data = 1234567;

    std::string str_data = "1234567";
    uint64_t data = 0;
    base::Code ret = GetUInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(small_data, data);
}/*}}}*/

TEST(GetUInt64, BigData)
{/*{{{*/
    using namespace base;

    uint64_t big_data = 0xffffffffffffffff;
    fprintf(stdout, "%#llx, %llu\n", big_data, big_data);

    std::string str_data = "18446744073709551615";
    uint64_t data = 0;
    base::Code ret = GetUInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(big_data, data);
}/*}}}*/
