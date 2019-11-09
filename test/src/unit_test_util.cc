// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/util.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(CheckAndGetIfIsAllNum, Test_Normal_Empty)
{/*{{{*/
    using namespace base;

    std::string num = " \r";
    bool is_num = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(false, is_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, Test_Normal_RealNum)
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

TEST(CheckAndGetIfIsAllNum, Test_Normal_RealPostNum)
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

TEST(CheckAndGetIfIsAllNum, Test_Normal_RealNegNum)
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

TEST(CheckAndGetIfIsAllNum, Test_Normal_RealNegNum_NoIsNegParam)
{/*{{{*/
    using namespace base;

    std::string num = "      -1234034590      ";
    std::string real_num = "1234034590";
    bool is_num = false;
    std::string post_num;
    Code ret = CheckAndGetIfIsAllNum(num, &is_num, &post_num, NULL);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(true, is_num);
    EXPECT_EQ(real_num, post_num);
}/*}}}*/

TEST(CheckAndGetIfIsAllNum, Test_Exception_RealNegNum)
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

TEST(CheckAndGetIfIsAllNum, Test_Exception_IllegalNum)
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

TEST(GetReadableStr, Test_Normal_SimpleStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    bin_str.append(1, 0x00);
    bin_str.append(1, 0x08);
    bin_str.append(1, 0x0f);
    bin_str.append(1, 0x3f);
    bin_str.append(1, 0x7f);
    bin_str.append(1, 0xed);
    bin_str.append(1, 0xf2);


    char buf[32] = {'0', '0', '0', '8', '0', 'f', '3', 'f', '7', 'f', 'e', 'd', 'f', '2'};
    std::string expected_readable_str(buf);

    std::string readable_str;
    Code ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);

    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetReadableStr, Test_Normal_AllBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    for (int i = 0; i < 256; ++i)
    {
        bin_str.append(1, i);
    }

    char buf[3] = {0};
    std::string expected_readable_str;
    for (int i = 0; i < 256; ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)i);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    Code ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);

    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetReadableStr, Test_Normal_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 1024;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetReadableStr, Test_Press_10_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 10;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetReadableStr(bin_str, &readable_str);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    // fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetReadableStr, Test_Press_100_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 100;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetReadableStr(bin_str, &readable_str);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    //fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetReadableStr, Test_Press_1000_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 1000;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetReadableStr(bin_str, &readable_str);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    //fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    ret = GetBinStr(readable_str, &tmp_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetBinStr, Test_Normal_SimpleStr)
{/*{{{*/
    using namespace base;
    std::string src_str = "0baf18ad030f1b3c9108765423dacbeffd";
    std::string bin_str;

    Code ret = GetBinStr(src_str, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(src_str.size(), bin_str.size()*2);

    std::string tmp_str;
    ret = GetReadableStr(bin_str, &tmp_str);
    EXPECT_EQ(kOk, ret);

    EXPECT_EQ(src_str, tmp_str);
}/*}}}*/

TEST(GetBinStr, Test_Exception_WrongLen)
{/*{{{*/
    using namespace base;
    std::string src_str = "0baf18ad030f1b3c9108765423dacbeff";
    std::string bin_str;

    Code ret = GetBinStr(src_str, &bin_str);
    EXPECT_EQ(kInvalidParam, ret);
}/*}}}*/

TEST(GetBinStr, Test_Press_10_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 10;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    // fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetBinStr(readable_str, &tmp_str);
    }
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetBinStr, Test_Press_100_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 100;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    // fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetBinStr(readable_str, &tmp_str);
    }
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/

TEST(GetBinStr, Test_Press_1000_Len_Ten_Thousand_RandomBinStr)
{/*{{{*/
    using namespace base;
    std::string bin_str;
    uint32_t len = 1000;
    Code ret = GetRandBinStr(len, &bin_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(len, bin_str.size());

    char buf[3] = {0};
    std::string expected_readable_str;
    for (size_t i = 0; i < bin_str.size(); ++i)
    {
        snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
        expected_readable_str.append(buf);
    }

    std::string readable_str;
    ret = GetReadableStr(bin_str, &readable_str);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(readable_str.size(), bin_str.size()*2);
    EXPECT_EQ(expected_readable_str, readable_str);
    // fprintf(stderr, "%s\n", readable_str.c_str());

    std::string tmp_str;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = GetBinStr(readable_str, &tmp_str);
    }
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(bin_str, tmp_str);
}/*}}}*/
