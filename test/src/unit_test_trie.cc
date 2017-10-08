// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <stdio.h>
#include <stdint.h>

#include "base/trie.h"
#include "base/common.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Trie, Test_Normal_Put)
{/*{{{*/
    using namespace base;

    Trie trie;
    Code ret = trie.Init();
    EXPECT_EQ(kOk, ret);

    std::string keys[] = {"bcd", "bce", "bcf"};
    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Put(keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(1, full_word_frequency);
    }

    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Put(keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(2, full_word_frequency);
    }

    std::string trie_info;
    ret = trie.ToString(&trie_info);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", trie_info.c_str());
}/*}}}*/

TEST(Trie, Test_Normal_Prefix_Put)
{/*{{{*/
    using namespace base;

    Trie trie;
    Code ret = trie.Init();
    EXPECT_EQ(kOk, ret);

    std::string prefix_keys[] = {"bcd", "bce", "bcf"};
    for (uint32_t i = 0; i < sizeof(prefix_keys)/sizeof(prefix_keys[0]); ++i)
    {
        ret = trie.Put(prefix_keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(prefix_keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(1, full_word_frequency);
    }

    std::string keys[] = {"bcdn", "bceh", "bcfm"};
    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Put(keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(1, full_word_frequency);
    }

    std::string trie_info;
    ret = trie.ToString(&trie_info);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", trie_info.c_str());
}/*}}}*/

TEST(Trie, Test_Normal_Prefix_Del)
{/*{{{*/
    using namespace base;

    Trie trie;
    Code ret = trie.Init();
    EXPECT_EQ(kOk, ret);

    std::string prefix_keys[] = {"bcd", "bce", "bcf"};
    for (uint32_t i = 0; i < sizeof(prefix_keys)/sizeof(prefix_keys[0]); ++i)
    {
        ret = trie.Put(prefix_keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(prefix_keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(1, full_word_frequency);
    }

    std::string keys[] = {"bcdn", "bceh", "bcfm"};
    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Put(keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(keys[i], &full_word_frequency);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(1, full_word_frequency);
    }

    for (uint32_t i = 0; i < sizeof(prefix_keys)/sizeof(prefix_keys[0]); ++i)
    {
        ret = trie.Del(prefix_keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(prefix_keys[i], &full_word_frequency);
        EXPECT_EQ(kNotFound, ret);
    }

    for (uint32_t i = 0; i < sizeof(prefix_keys)/sizeof(prefix_keys[0]); ++i)
    {
        ret = trie.Del(prefix_keys[i]);
        EXPECT_EQ(kNotFound, ret);
    }

    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Del(keys[i]);
        EXPECT_EQ(kOk, ret);

        uint32_t full_word_frequency = 0;
        ret = trie.Get(keys[i], &full_word_frequency);
        EXPECT_EQ(kNotFound, ret);
    }

    for (uint32_t i = 0; i < sizeof(keys)/sizeof(keys[0]); ++i)
    {
        ret = trie.Del(keys[i]);
        EXPECT_EQ(kNotFound, ret);
    }

    std::string trie_info;
    ret = trie.ToString(&trie_info);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", trie_info.c_str());
}/*}}}*/
