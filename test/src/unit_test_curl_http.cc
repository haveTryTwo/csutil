// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "base/status.h"
#include "base/common.h"
#include "base/curl_http.h"

#include "test_base/include/test_base.h"

TEST(CurlHttp, Test_Normal_Post)
{/*{{{*/
    using namespace base;

    CurlHttp curl_http;
    Code r = curl_http.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string content;
    std::string result;

    r = curl_http.Post(url, content, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, content:%s, ret:%d\n",
            url.c_str(), content.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(CurlHttp, Test_Normal_Get)
{/*{{{*/
    using namespace base;

    CurlHttp curl_http;
    Code r = curl_http.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string result;

    r = curl_http.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, ret:%d\n",
            url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
}/*}}}*/

TEST(CurlHttp, Test_Normal_Get_Localhost)
{/*{{{*/
    using namespace base;

    CurlHttp curl_http;
    Code r = curl_http.Init();
    EXPECT_EQ(kOk, r);

    //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "localhost:9123";

    while (true) 
    {
        std::string result;
        r = curl_http.Get(url, &result);
        EXPECT_EQ(kOk, r);
        if (r != kOk)
        {
            fprintf(stderr, "Failed to get result of url:%s, ret:%d\n",
                    url.c_str(), r);
        }

        fprintf(stderr, "%s\n", result.c_str());
        usleep(1000);
    }
}/*}}}*/

