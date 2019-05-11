// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <bitset>

#include <stdio.h>
#include <stdint.h>

#include "base/md5.h"
#include "base/coding.h"
#include "base/status.h"
#include "base/message_digest.h"

#include "test_base/include/test_base.h"

TEST(Md5, Test_Normal_Str)
{/*{{{*/
    using namespace base;

    MessageDigest *msg_digest = new Md5();
    EXPECT_NEQ(NULL, msg_digest);

    Code ret = msg_digest->Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "abc\n";
    std::string standard_md5_value = "0bee89b07a248e27c83fc3d5951213c1"; // readable string value of md5

    std::string value;
    ret = msg_digest->Sum(str, &value);
    EXPECT_EQ(kOk, ret);

    char buf[33] = {0};
    for (uint32_t i = 0; i < value.size(); ++i)
    {
        snprintf(buf+i*2, 33-i*2, "%02x", (uint8_t)value.data()[i]);
    }
    std::string readable_value;
    readable_value.assign(buf, 32);
    EXPECT_EQ(standard_md5_value, readable_value);

    fprintf(stderr, "standard:%s, size:%zu, current:%s, size:%zu\n", 
        standard_md5_value.c_str(), standard_md5_value.size(),
        readable_value.c_str(), readable_value.size());

    delete msg_digest;
}/*}}}*/

TEST(Md5, Test_Normal_Batch_Str)
{/*{{{*/
    using namespace base;

    MessageDigest *msg_digest = new Md5();
    EXPECT_NEQ(NULL, msg_digest);

    Code ret = msg_digest->Init();
    EXPECT_EQ(kOk, ret);

    std::map<std::string, std::string> md5_maps;
    md5_maps[""] = "d41d8cd98f00b204e9800998ecf8427e";
    md5_maps["a"] = "0cc175b9c0f1b6a831c399e269772661"; 
    md5_maps["ab"] = "187ef4436122d1cc2f40dc2b92f0eba0";
    md5_maps["abc"] = "900150983cd24fb0d6963f7d28e17f72";
    md5_maps["abcd"] = "e2fc714c4727ee9395f324cd2e7f331f";
    md5_maps["abcde"] = "ab56b4d92b40713acc5af89985d4b786";
    md5_maps["abcdef"] = "e80b5017098950fc58aad83c8c14978e";
    md5_maps["abcdefg"] = "7ac66c0f148de9519b8bd264312c4d64";
    md5_maps["abcdefgh"] = "e8dc4081b13434b45189a720b77b6818";
    md5_maps["abcdefghi"] = "8aa99b1f439ff71293e95357bac6fd94";
    md5_maps["abc\n"] = "0bee89b07a248e27c83fc3d5951213c1";
    md5_maps["123456789abcdef"] = "649f312aaffac7561ae81f8f9c334f14";
    md5_maps["xxxxxxxxxxxxxxxx"] = "45ed9cc2f92b77cd8b2f5bd59ff635f8";
    md5_maps["@@@@@@@#######******MMMM"] = "3c757de8cda7b3bc8fcf570a0803819b";
    md5_maps["((((((asdfasoi32948723451198073@#&&*@#@#897asdfagmmxncvvbkjy3219782345sadfMSDF23452191234))))))"] = "ee2062425f79d8b7f8e8c97d5279a24d";

    std::map<std::string, std::string>::iterator it;
    for (it = md5_maps.begin(); it != md5_maps.end(); ++it)
    {
        std::string value;
        ret = msg_digest->Sum(it->first, &value);
        EXPECT_EQ(kOk, ret);

        char buf[33] = {0};
        for (uint32_t i = 0; i < value.size(); ++i)
        {
            snprintf(buf+i*2, 33-i*2, "%02x", (uint8_t)value.data()[i]);
        }
        std::string readable_value;
        readable_value.assign(buf, 32);
        EXPECT_EQ(it->second, readable_value);

        fprintf(stderr, "key:%s, standard:%s, current:%s\n",
            it->first.c_str(), it->second.c_str(), readable_value.c_str());
    }

    delete msg_digest;
}/*}}}*/

TEST(Md5, Test_Press_1M)
{/*{{{*/
    using namespace base;

    MessageDigest *msg_digest = new Md5();
    EXPECT_NEQ(NULL, msg_digest);

    Code ret = msg_digest->Init();
    EXPECT_EQ(kOk, ret);

    std::string str = "153318220032000001";
    std::string standard_md5_value = "9be33ce8d07546c3506e750ee00f0f91"; // readable string value of md5

    std::string value;
    for (int i = 0; i < 1000000; ++i)
    {
        ret = msg_digest->Sum(str, &value);
        EXPECT_EQ(kOk, ret);
    }

    char buf[33] = {0};
    for (uint32_t i = 0; i < value.size(); ++i)
    {
        snprintf(buf+i*2, 33-i*2, "%02x", (uint8_t)value.data()[i]);
    }

    std::string readable_value;
    readable_value.assign(buf, 32);
    EXPECT_EQ(standard_md5_value, readable_value);

    fprintf(stderr, "standard:%s, size:%zu, current:%s, size:%zu\n", 
        standard_md5_value.c_str(), standard_md5_value.size(),
        readable_value.c_str(), readable_value.size());

    delete msg_digest;
}/*}}}*/
