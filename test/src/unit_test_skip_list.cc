// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <string.h>

#include "base/util.h"
#include "base/common.h"
#include "base/status.h"
#include "base/skip_list.h"

#include "test_base/include/test_base.h"

class CompareInt
{
    public:
        int operator() (int first, int second)
        {
            return first-second;
        }
};

class CompareString
{
    public:
        int operator() (const std::string &first, const std::string &second)
        {
            return first.compare(second);
        }
};

TEST(SkipLit, Test_Normal_Get_String)
{/*{{{*/
    using namespace base;

    SkipList<std::string, std::string, CompareString> skip_list;
    Code ret = skip_list.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t max_num = 100;
    char buf[8] = "\0";

    // Put
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;

        Code ret = skip_list.Put(key, value);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Put(key, value);
        EXPECT_EQ(kExist, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        if (ret != kOk) fprintf(stderr, "<%s, %s>, ret:%d\n", key.c_str(), value.c_str(), ret);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(value, tmp_value);
    }/*}}}*/


    SkipList<std::string, std::string, CompareString>::Iterator iter(&skip_list);
    for (iter.SetFirst(); iter.Valid(); iter.GoNext())
    {/*{{{*/
        std::string key;
        std::string value;
        int height;
        Code ret = iter.GetKey(&key);
        EXPECT_EQ(kOk, ret);
        ret = iter.GetValue(&value);
        EXPECT_EQ(kOk, ret);
        ret = iter.GetHeight(&height);
        EXPECT_EQ(kOk, ret);

        fprintf(stderr, "<%s, %s, %d>\n", key.c_str(), value.c_str(), height);
    }/*}}}*/

}/*}}}*/

TEST(SkipLit, Test_Normal_Del_String)
{/*{{{*/
    using namespace base;

    SkipList<std::string, std::string, CompareString> skip_list;
    Code ret = skip_list.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t max_num = 100;
    char buf[8] = "\0";

    // Put
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;

        Code ret = skip_list.Put(key, value);
        EXPECT_EQ(kOk, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(value, tmp_value);
    }/*}}}*/

    // Del
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        Code ret = skip_list.Del(key);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Del(key);
        EXPECT_EQ(kNotFound, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        EXPECT_EQ(kNotFound, ret);
    }/*}}}*/

}/*}}}*/

TEST(SkipLit, Test_Normal_Get_Int)
{/*{{{*/
    using namespace base;

    SkipList<int, std::string, CompareInt> skip_list;
    Code ret = skip_list.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t max_num = 100;
    char buf[8] = "\0";

    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string value = std::string("value") + buf;
        Code ret = skip_list.Put(key, value);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Put(key, value);
        EXPECT_EQ(kExist, ret);
    }/*}}}*/

    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(value, tmp_value);
    }/*}}}*/


    SkipList<int, std::string, CompareInt>::Iterator iter(&skip_list);
    for (iter.SetFirst(); iter.Valid(); iter.GoNext())
    {/*{{{*/
        int key;
        std::string value;
        int height;
        Code ret = iter.GetKey(&key);
        EXPECT_EQ(kOk, ret);
        ret = iter.GetValue(&value);
        EXPECT_EQ(kOk, ret);
        ret = iter.GetHeight(&height);
        EXPECT_EQ(kOk, ret);

//        fprintf(stderr, "<%d, %s, %d>\n", key, value.c_str(), height);
    }/*}}}*/

}/*}}}*/

TEST(SkipLit, Test_Normal_Del_Int)
{/*{{{*/
    using namespace base;

    SkipList<int, std::string, CompareInt> skip_list;
    Code ret = skip_list.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t max_num = 100;
    char buf[8] = "\0";

    // Put
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string value = std::string("value") + buf;
        Code ret = skip_list.Put(key, value);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Put(key, value);
        EXPECT_EQ(kExist, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(value, tmp_value);
    }/*}}}*/

    // Del
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        Code ret = skip_list.Del(key);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Del(key);
        EXPECT_EQ(kNotFound, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        int key = i;
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        EXPECT_EQ(kNotFound, ret);
    }/*}}}*/

}/*}}}*/

TEST(SkipLit, Test_Press_Ten_Thousand_String)
{/*{{{*/
    using namespace base;

    SkipList<std::string, std::string, CompareString> skip_list;
    Code ret = skip_list.Init();
    EXPECT_EQ(kOk, ret);

    uint32_t max_num = 100000;
    char buf[8] = "\0";

    // Put
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;

        Code ret = skip_list.Put(key, value);
        EXPECT_EQ(kOk, ret);

        ret = skip_list.Put(key, value);
        EXPECT_EQ(kExist, ret);
    }/*}}}*/

    // Get
    for (uint32_t i = 0; i < max_num; ++i)
    {/*{{{*/
        snprintf(buf, sizeof(buf), "%u", (unsigned int)i); 
        std::string key = std::string("key") + buf;
        std::string value = std::string("value") + buf;
        std::string tmp_value;

        Code ret = skip_list.Get(key, &tmp_value);
        if (ret != kOk) fprintf(stderr, "<%s, %s>, ret:%d\n", key.c_str(), value.c_str(), ret);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(value, tmp_value);
    }/*}}}*/

}/*}}}*/
