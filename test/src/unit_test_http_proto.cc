// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "base/status.h"
#include "base/common.h"
#include "http/http_proto.h"

#include "test_base/include/test_base.h"

TEST(HttpProto, Test_Normal_Encodeeq_GET)
{/*{{{*/
    using namespace base;
    std::string url = "http://1.1.1.1:13459/csutil/test.html?Name=LiSi&Age=11&Height=1.1";
    HttpProto http_proto;
    Code ret = http_proto.SetHttpType(base::GET);
    EXPECT_EQ(kOk, ret);

    std::string stream_data;
    std::string host;
    uint16_t port = 0;
    ret = http_proto.EncodeToReq(url, &stream_data, &host, &port);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ("1.1.1.1", host);
    EXPECT_EQ(13459, port);
    fprintf(stderr, "%s\n", stream_data.c_str());
}/*}}}*/

TEST(HttpProto, Test_Normal_EncodeReq_POST)
{/*{{{*/
    using namespace base;
    std::string url = "http://1.1.1.1:13459/csutil/test.html?Name=LiSi&Age=11&Height=1.1";
    HttpProto http_proto;
    Code ret = http_proto.SetHttpType(base::POST);
    EXPECT_EQ(kOk, ret);

    std::string stream_data;
    std::string host;
    uint16_t port = 0;
    ret = http_proto.EncodeToReq(url, &stream_data, &host, &port);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ("1.1.1.1", host);
    EXPECT_EQ(13459, port);
    fprintf(stderr, "%s\n", stream_data.c_str());
}/*}}}*/

TEST(HttpProto, Test_Normal_EncodeReq_POST_With_POST_Params)
{/*{{{*/
    using namespace base;
    std::string post_params = "Key=id&Value=101";
    std::string url = "http://1.1.1.1:13459/csutil/test.html?Name=LiSi&Age=11&Height=1.1";
    HttpProto http_proto;
    Code ret = http_proto.SetHttpType(base::POST);
    EXPECT_EQ(kOk, ret);

    std::string stream_data;
    std::string host;
    uint16_t port = 0;
    ret = http_proto.EncodeToReq(url, post_params, &stream_data, &host, &port);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ("1.1.1.1", host);
    EXPECT_EQ(13459, port);
    fprintf(stderr, "%s\n", stream_data.c_str());
}/*}}}*/
