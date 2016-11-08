// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
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
