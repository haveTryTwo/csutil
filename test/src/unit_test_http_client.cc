// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "base/status.h"
#include "base/common.h"
#include "http/http_client.h"

#include "test_base/include/test_base.h"

TEST(HttpClient, Test_Normal_Post)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string post_params;
    std::string result;

    r = http_client.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r); // TODO: considering r: kHttpStatusRedirect             = 302, // Move temporarily
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n",
            url.c_str(), post_params.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(HttpClient, Test_Normal_Get)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string result;

    r = http_client.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, ret:%d\n",
            url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(HttpClient, Test_Normal_Get_Localhost)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "localhost:9123";

    while (true) 
    {
        std::string result;
        r = http_client.Get(url, &result);
        EXPECT_EQ(kOk, r);
        if (r != kOk)
        {
            fprintf(stderr, "Failed to get result of url:%s, ret:%d\n",
                url.c_str(), r);
        }

        fprintf(stderr, "%s\n", result.c_str());
        usleep(1000);
        break;
    }
}/*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Bulk)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://localhost:9200/_bulk?pretty";
    std::string post_params =
        "{ \"delete\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
        "{ \"create\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
        "{ \"name\" : \"EEEEEAA\", \"country\" : \"China\", \"age\" : 129}\n"
        "{ \"index\":  { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 12 }}\n"
        "{ \"name\" : \"FFFFFBB\", \"country\" : \"US\", \"age\" : 128}\n"
        "{ \"update\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 13, \"_retry_on_conflict\" : 3} }\n"
        "{ \"doc\" : {\"title\" : \"Very good and yes\"}, \"doc_as_upsert\": true}\n";
    std::string result;

    r = http_client.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n",
            url.c_str(), post_params.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Search)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://localhost:9200/tests_3/_search?pretty";
    std::string post_params = "{}";
    std::string result;

    r = http_client.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n",
            url.c_str(), post_params.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(HttpClient, Test_Normal_ES_Get_ID)
{/*{{{*/
    using namespace base;

    HttpClient http_client;
    Code r = http_client.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://localhost:9200/tests_3/_doc/11?pretty";
    std::string result;

    r = http_client.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/
