// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/status.h"

#include "test_base/include/test_base.h"

#include "store/cache/lru_cache/src/lru_cache.h"

TEST(LruCache, NormalDataGet)
{/*{{{*/
    using namespace base;
    using namespace store;

    LRUCache lru_cache;

    std::string key1 = "key1";
    std::string value1 = "value1";
    std::string key2 = "key2";
    std::string value2 = "value2";

    Code ret = lru_cache.Put(key1, value1);
    EXPECT_EQ(kOk, ret);
    ret = lru_cache.Put(key2, value2);
    EXPECT_EQ(kOk, ret);

    std::string tmp_value1;
    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value1, tmp_value1);

    std::string tmp_value2;
    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value2, tmp_value2);
}/*}}}*/

TEST(LruCache, NormalDataGetOutForMaxNum)
{/*{{{*/
    using namespace base;
    using namespace store;

    uint32_t max_num = 1;
    uint32_t time_interval = 0;
    LRUCache lru_cache;
    lru_cache.Init(max_num, time_interval);

    std::string key1 = "key1";
    std::string value1 = "value1";
    std::string key2 = "key2";
    std::string value2 = "value2";

    // insert key1
    Code ret = lru_cache.Put(key1, value1);
    EXPECT_EQ(kOk, ret);

    std::string tmp_value1;
    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value1, tmp_value1);

    // insert key2
    ret = lru_cache.Put(key2, value2);
    EXPECT_EQ(kOk, ret);

    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kNotFound, ret);

    std::string tmp_value2;
    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value2, tmp_value2);
}/*}}}*/

TEST(LruCache, NormalDataGetOutForTimeInterval)
{/*{{{*/
    using namespace base;
    using namespace store;

    uint32_t max_num = 0;
    uint32_t time_interval = 1;
    LRUCache lru_cache;
    lru_cache.Init(max_num, time_interval);

    std::string key1 = "key1";
    std::string value1 = "value1";
    std::string key2 = "key2";
    std::string value2 = "value2";

    // insert key1
    Code ret = lru_cache.Put(key1, value1);
    EXPECT_EQ(kOk, ret);

    std::string tmp_value1;
    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value1, tmp_value1);

    // insert key2
    ret = lru_cache.Put(key2, value2);
    EXPECT_EQ(kOk, ret);

    std::string tmp_value2;
    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value2, tmp_value2);

    // sleep 2 seconds for time interval
    sleep(2);

    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kNotFound, ret);

    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kNotFound, ret);
}/*}}}*/

TEST(LruCache, NormalDataDel)
{/*{{{*/
    using namespace base;
    using namespace store;

    LRUCache lru_cache;

    std::string key1 = "key1";
    std::string value1 = "value1";
    std::string key2 = "key2";
    std::string value2 = "value2";

    // insert key1
    Code ret = lru_cache.Put(key1, value1);
    EXPECT_EQ(kOk, ret);
    // insert key2
    ret = lru_cache.Put(key2, value2);
    EXPECT_EQ(kOk, ret);

    std::string tmp_value1;
    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value1, tmp_value1);

    std::string tmp_value2;
    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(value2, tmp_value2);

    // delete key1
    ret = lru_cache.Del(key1);
    EXPECT_EQ(kOk, ret);
    ret = lru_cache.Get(key1, &tmp_value1);
    EXPECT_EQ(kNotFound, ret);

    // delete key2
    ret = lru_cache.Del(key2);
    EXPECT_EQ(kOk, ret);
    ret = lru_cache.Get(key2, &tmp_value2);
    EXPECT_EQ(kNotFound, ret);
}/*}}}*/

TEST(LruCache, BigDataGetOutForMaxNum)
{/*{{{*/
    using namespace base;
    using namespace store;

    uint32_t max_num = 100000;
    uint32_t time_interval = 0;
    LRUCache lru_cache;
    lru_cache.Init(max_num, time_interval);

    char buf[8] = "\0";
    for (uint32_t i = 0; i < max_num * 2; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%d", i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;

        Code ret = lru_cache.Put(key, value);
        EXPECT_EQ(kOk, ret);
    }/*}}}*/

    for (uint32_t i = 0; i < max_num * 2; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%d", i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = lru_cache.Get(key, &tmp_value);
        if (i < max_num)
        {
            EXPECT_EQ(kNotFound, ret);
        }
        else
        {
            EXPECT_EQ(kOk, ret);
            EXPECT_EQ(value, tmp_value);
        }
    }/*}}}*/
}/*}}}*/
