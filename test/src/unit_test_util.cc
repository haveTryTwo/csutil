// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/util.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(CheckAndGetIfIsAllNum, TestEmpty)
{/*{{{*/
    using namespace base;

    std::string num = " \r";
    bool is_num = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, is_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, TestRealNum)
{/*{{{*/
    using namespace base;

    std::string num = "      1234034590      ";
    std::string real_num = "1234034590";
    bool is_num = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
    EXPECT_EQ(real_num, post_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, TestRealPostNum)
{/*{{{*/
    using namespace base;

    std::string num = "      +1234034590      ";
    std::string real_num = "1234034590";
    bool is_num = false;
    bool is_negv = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num, &is_negv);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
    EXPECT_EQ(false, is_negv);
    EXPECT_EQ(real_num, post_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, TestRealNegNum)
{/*{{{*/
    using namespace base;

    std::string num = "      -1234034590      ";
    std::string real_num = "1234034590";
    bool is_num = false;
    bool is_negv = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num, &is_negv);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
    EXPECT_EQ(true, is_negv);
    EXPECT_EQ(real_num, post_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, TestExceptionRealNegNum)
{/*{{{*/
    using namespace base;

    std::string num = "      -1234-034590      ";
    bool is_num = false;
    bool is_negv = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num, &is_negv);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, is_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, TestExceptionIllegalNum)
{/*{{{*/
    using namespace base;

    std::string num = "      -1234MH034590      ";
    bool is_num = false;
    bool is_negv = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num, &is_negv);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, is_num);
}/*}}}*/

TEST(CheckIsNum, TestEmpty)
{/*{{{*/
    using namespace base;

    std::string num = " \r";
    bool is_num = false;
    Code ret = CheckIsAllNum(num, &is_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, is_num);
}/*}}}*/

TEST(CheckIsNum, TestRealNum)
{/*{{{*/
    using namespace base;

    std::string num = "      1234034590      ";
    bool is_num = false;
    Code ret = CheckIsAllNum(num, &is_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
}/*}}}*/

TEST(CheckIsNum, TestRealPostNum)
{/*{{{*/
    using namespace base;

    std::string num = "      +1234034590      ";
    bool is_num = false;
    Code ret = CheckIsAllNum(num, &is_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
}/*}}}*/

TEST(CheckIsNum, TestRealNegNum)
{/*{{{*/
    using namespace base;

    std::string num = "      -1234034590      ";
    bool is_num = false;
    bool is_negv = false;
    Code ret = CheckIsAllNum(num, &is_num, &is_negv);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
    EXPECT_EQ(true, is_negv);
}/*}}}*/

TEST(Reverse, TestNULL)
{/*{{{*/
    using namespace base;

    std::string src = "abcdef";
    Code ret = Reverse(src, NULL);
    EXPECT_EQ(kInvalidParam, ret);
}/*}}}*/

TEST(Reverse, TestNormal)
{/*{{{*/
    using namespace base;

    std::string src = "abcdef";
    std::string rev_str = "fedcba";
    std::string dst;
    Code ret = Reverse(src, &dst);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(rev_str, dst);
}/*}}}*/

TEST(GetOutputSuffix, NumB)
{/*{{{*/
    using namespace base;

    uint64_t num = 502;
    float out_num = 502.0;
    std::string suffix = "B";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumKB)
{/*{{{*/
    using namespace base;

    uint64_t num = 502 * 1024;
    float out_num = 502.0;
    std::string suffix = "KB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumMB)
{/*{{{*/
    using namespace base;

    uint64_t num = 502 * 1024 * 1024;
    float out_num = 502.0;
    std::string suffix = "MB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumGB)
{/*{{{*/
    using namespace base;

    uint64_t num = (uint64_t)502 * 1024 * 1024 * 1024;
    float out_num = 502.0;
    std::string suffix = "GB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumTB)
{/*{{{*/
    using namespace base;

    uint64_t num = (uint64_t)502 * 1024 * 1024 * 1024 * 1024;
    float out_num = 502.0;
    std::string suffix = "TB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumPB)
{/*{{{*/
    using namespace base;

    uint64_t num = (uint64_t)502 * 1024 * 1024 * 1024 * 1024 * 1024;
    float out_num = 502.0;
    std::string suffix = "PB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

TEST(GetOutputSuffix, NumGreaterThanPB)
{/*{{{*/
    using namespace base;

    uint64_t num = (uint64_t)2 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
    float out_num = 2.0 * 1024;
    std::string suffix = "PB";
    float tmp_out_num = 0;
    std::string tmp_suffix;

    Code ret = GetOutputSuffix(num, &tmp_out_num, &tmp_suffix);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(out_num, tmp_out_num);
    EXPECT_EQ(suffix, tmp_suffix);
}/*}}}*/

