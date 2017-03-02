// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <string.h>

#include "base/reg.h"
#include "base/util.h"
#include "base/common.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Reg, Normal_HorizontalLine)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[a-]$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "-";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(Reg, Normal_JoinerLine)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[a-h1-9A-E]$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "-";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    for (char ch = 'a'; ch <= 'h'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'i'; ch <= 'z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = '1'; ch <= '9'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'A'; ch <= 'E'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = 'F'; ch <= 'Z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }
}/*}}}*/

TEST(Reg, Normal_NonBrackets)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[^a-h1-9A-E]$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (char ch = 'a'; ch <= 'h'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'i'; ch <= 'z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }

    for (char ch = '1'; ch <= '9'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'A'; ch <= 'E'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kRegNotMatch, ret);
    }

    for (char ch = 'F'; ch <= 'Z'; ++ch)
    {
        str.assign(1, ch);
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/

TEST(Reg, Normal_PartReg)
{/*{{{*/
    using namespace base;

    std::string reg_str = "test";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "justinesto";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "justintesto";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(Reg, Normal_PartBranch)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^ab|cd$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "bg";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ffcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "cdcdcdcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(Reg, Normal_PartBranchRepeated)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)+|cd$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "abababab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ababababc";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "bg";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ffcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "cdcdcdcd";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(Reg, Normal_WholePlusRepeated)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^(ab)+$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abm";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "ab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "abababab";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "ababababc";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "bgcd";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);
}/*}}}*/

TEST(Reg, Normal_HttpCheck)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^http://([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)+(:[0-9]+)?(/[a-zA-Z0-9]*)*(\\?[a-zA-Z0-9=]*)?$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "htt://zhangd";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.havetrytwo.:8090/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.havetrytwo:aa90/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kRegNotMatch, ret);

    str = "http://www.sz.gov.cn/cn/hdjl/zxts/dfyj/";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);

    str = "http://www.havetrytwo:8090/index?name=good";
    ret = reg.Check(str);
    EXPECT_EQ(kOk, ret);
}/*}}}*/

TEST(Reg, Normal_Press_OneHoudredThousandTimes)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^http://([a-zA-Z0-9]+)(\\.[a-zA-Z0-9]+)+(:[0-9]+)?(/[a-zA-Z0-9]*)*(\\?[a-zA-Z0-9=]*)?$";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (uint32_t i = 0; i < kMillion/10; ++i)
    {
        str = "http://www.havetrytwo:8090/index?name=good";
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/

TEST(Reg, Normal_Press_OneMillionTimes)
{/*{{{*/
    using namespace base;

    std::string reg_str = "^[hH][tT][tT][pP]/[0-9]\\.[0-9] [0-9][0-9][0-9] .*";
    Reg reg(reg_str);
    Code ret = reg.Init();
    EXPECT_EQ(kOk, ret);

    std::string str;
    for (uint32_t i = 0; i < kMillion; ++i)
    {
        str = "http/1.1 200 OK";
        ret = reg.Check(str);
        EXPECT_EQ(kOk, ret);
    }
}/*}}}*/
