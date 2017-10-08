// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <stdio.h>
#include <stdint.h>

#include "base/hash.h"
#include "base/common.h"
#include "base/status.h"
#include "base/bloom_filter.h"

#include "test_base/include/test_base.h"

TEST(BloomFilter, Test_Normal_Put)
{/*{{{*/
    using namespace base;

    uint32_t bits_per_key = 10;
    uint32_t keys_num = 10;
    uint32_t hash_num = 3;

    BloomFilter bloom(bits_per_key, keys_num, hash_num);
    Code ret = bloom.Init();
    EXPECT_EQ(kOk, ret);

    std::string keys[] = {"nice", "That is", "ok"};
    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = bloom.Put(keys[i]);
        EXPECT_EQ(kOk, ret);

        bool exist = false;
        ret = bloom.CheckExist(keys[i], &exist);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(true, exist);
    }

    std::string bloom_arr;
    ret = bloom.GetBytesStr(&bloom_arr);
    EXPECT_EQ(kOk, ret);
    for (uint32_t i = 0; i < bloom_arr.size(); ++i)
    {
        fprintf(stderr, "%02x", (uint8_t)bloom_arr[i]);
    }
    fprintf(stderr, "\n");
}/*}}}*/

TEST(BloomFilter, Test_Exception_Check)
{/*{{{*/
    using namespace base;

    uint32_t bits_per_key = 10;
    uint32_t keys_num = 10;
    uint32_t hash_num = 3;

    BloomFilter bloom(bits_per_key, keys_num, hash_num);
    Code ret = bloom.Init();
    EXPECT_EQ(kOk, ret);

    std::string keys[] = {"nice", "That is", "ok"};
    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        bool exist = false;
        ret = bloom.CheckExist(keys[i], &exist);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(false, exist);
    }

    ret = bloom.Put("Num");
    EXPECT_EQ(kOk, ret);

    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        bool exist = false;
        ret = bloom.CheckExist(keys[i], &exist);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(false, exist);
    }

}/*}}}*/

TEST(BloomFilter, Test_Press_MillonKeys)
{/*{{{*/
    using namespace base;

    uint32_t bits_per_key = 10;
    uint32_t keys_num = base::kMillion;
    uint32_t hash_num = 3;

    BloomFilter bloom(bits_per_key, keys_num, hash_num);
    Code ret = bloom.Init();
    EXPECT_EQ(kOk, ret);

    char buf[8] = "\0";
    for (uint32_t i = 0; i < keys_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;

        ret = bloom.Put(key);
        EXPECT_EQ(kOk, ret);
    }/*}}}*/

    for (uint32_t i = 0; i < keys_num; ++i)
    {
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;

        bool exist = false;
        ret = bloom.CheckExist(key, &exist);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(true, exist);
    }

    {
        snprintf(buf, sizeof(buf), "%u", base::kMillion); 
        std::string key = std::string("key") + buf;

        bool exist = false;
        ret = bloom.CheckExist(key, &exist);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(false, exist);
    }
}/*}}}*/
