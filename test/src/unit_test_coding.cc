// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <bitset>

#include <stdio.h>
#include <stdint.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Base64Encode, Test_Normal_Str)
{/*{{{*/
    using namespace base;

    std::map<std::string, std::string> maps;

    maps["abc"] = "YWJj";
    maps["abcd"] = "YWJjZA==";
    maps["abcde"] = "YWJjZGU=";
    maps["abcdef"] = "YWJjZGVm";
    maps["azAZ09+/"] = "YXpBWjA5Ky8=";
    maps["   "] = "ICAg";
    maps["abcdefEG m%+@@#!%#% ABVC"] = "YWJjZGVmRUcgbSUrQEAjISUjJSBBQlZD";
    
    std::string tmp_src;
    std::string tmp_dst;
    std::map<std::string, std::string>::iterator it;
    for (it = maps.begin(); it != maps.end(); ++it)
    {
        Code ret = Base64Encode(it->first, &tmp_dst);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(it->second, tmp_dst);
//        fprintf(stderr, "%s : %s\n", it->second.c_str(), tmp_dst.c_str());
        
        ret = Base64Decode(tmp_dst, &tmp_src);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(it->first, tmp_src);
//        fprintf(stderr, "%s : %s\n", it->first.c_str(), tmp_src.c_str());
    }

}/*}}}*/

TEST(Base64Encode, Test_Normal_Full_Char)
{/*{{{*/
    using namespace base;

    std::string src;
    for (uint32_t i = 0; i < 256; ++i)
    {
        src.append(1, i);
    }

    std::string tmp_src;
    std::string tmp_dst;

    Code ret = Base64Encode(src, &tmp_dst);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", tmp_dst.c_str());

    ret = Base64Decode(tmp_dst, &tmp_src);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(src, tmp_src);
}/*}}}*/

TEST(Base64Decode, Test_Exception_Wrong_Str)
{/*{{{*/
    using namespace base;

    std::string encode_str;
    for (uint32_t i = 211; i < 256; ++i)
    {
        encode_str.append(1, i);
    }

    std::string tmp_src;
    Code ret = Base64Decode(encode_str, &tmp_src);
    EXPECT_EQ(kInvalidParam, ret);

    encode_str = "abce=";
    ret = Base64Decode(encode_str, &tmp_src);
    EXPECT_EQ(kInvalidParam, ret);

    encode_str = "ab=e";
    ret = Base64Decode(encode_str, &tmp_src);
    EXPECT_EQ(kInvalidParam, ret);

    encode_str = "a=ce";
    ret = Base64Decode(encode_str, &tmp_src);
    EXPECT_EQ(kInvalidParam, ret);

    encode_str = "=ace";
    ret = Base64Decode(encode_str, &tmp_src);
    EXPECT_EQ(kInvalidParam, ret);
}/*}}}*/
