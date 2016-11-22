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

    int64_t zero_data = 0;

    std::string str_data = "0";
    int64_t data = 0;
    base::Code ret = GetInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(zero_data, data);
}/*}}}*/

TEST(GetInt64, SmallData)
{/*{{{*/
    using namespace base;

    int64_t small_data = 1234567;

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
    int64_t big_data = 0xffffffffffffffff;
    fprintf(stdout, "%#llx, %llu\n", (unsigned long long)big_data, (unsigned long long)big_data);

    std::string str_data = "18446744073709551615";
//    str_data = "9223372036854775808";
    int64_t data = 0;
    base::Code ret = GetInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(big_data, data);
    fprintf(stdout, "%#llx, %llu\n", (unsigned long long)data, (unsigned long long)data);
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
    fprintf(stdout, "%#llx, %llu\n", (unsigned long long)big_data, (unsigned long long)big_data);

    std::string str_data = "18446744073709551615";
    uint64_t data = 0;
    base::Code ret = GetUInt64(str_data, &data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(big_data, data);
}/*}}}*/

TEST(GetUInt64, ExceptionOverflow)
{/*{{{*/
    using namespace base;

    std::string str_data = "2184467440737095516151";
    uint64_t data = 0;
    base::Code ret = GetUInt64(str_data, &data);
    EXPECT_EQ(kStrtollFailed, ret);
}/*}}}*/

TEST(BigAdd, NormalEqualLenth)
{/*{{{*/
    using namespace base;

    std::string ln          = "1234123412341234123412341234";
    std::string rn          = "1234123412341234123412341234";
    std::string real_sum    = "2468246824682468246824682468";
    std::string result;
    Code ret = BigAdd(ln, rn, &result);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(real_sum, result);
}/*}}}*/

TEST(BigAdd, NormalEqualLenthWithCarry)
{/*{{{*/
    using namespace base;

    std::string ln          = " 9768123412341234123412374634";
    std::string rn          = " 3959123412341234123412399834";
    std::string real_sum    = "13727246824682468246824774468";
    std::string result;
    Code ret = BigAdd(ln, rn, &result);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(real_sum, result);
}/*}}}*/

TEST(BigAdd, NormalLeftLongLenth)
{/*{{{*/
    using namespace base;

    std::string ln          = " 9968123412341234123412374634";
    std::string rn          = "   59123412341234123412399834";
    std::string real_sum    = "10027246824682468246824774468";
    std::string result;
    Code ret = BigAdd(ln, rn, &result);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(real_sum, result);
}/*}}}*/

TEST(BigAdd, NormalRightLongLenth)
{/*{{{*/
    using namespace base;

    std::string ln          = "   59123412341234123412399834";
    std::string rn          = " 9968123412341234123412374634";
    std::string real_sum    = "10027246824682468246824774468";
    std::string result;
    Code ret = BigAdd(ln, rn, &result);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(real_sum, result);
}/*}}}*/

TEST(BigAdd,ExceptionNum) 
{/*{{{*/
    using namespace base;

    std::string ln          = "   59a123412341234123412399834";
    std::string rn          = " 9968a123412341234123412374634";
    std::string result;
    Code ret = BigAdd(ln, rn, &result);
    EXPECT_EQ(kNotAllDigits, ret);
}/*}}}*/
