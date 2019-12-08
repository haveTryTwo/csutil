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

TEST(HttpProto, Test_Normal_EncodeReq_GET)
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

    // HttpReqProtoFunc
    int real_len = 0;
    ret = HttpReqProtoFunc(stream_data.data(), stream_data.size(), &real_len);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(stream_data.size(), real_len);

    HttpProto req_proto;
    ret = req_proto.DecodeFromReq(stream_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(base::GET, req_proto.GetHttpType());
    EXPECT_EQ("HTTP/1.1", req_proto.GetProtoVersion());
    EXPECT_EQ("1.1.1.1", req_proto.GetHost());
    EXPECT_EQ(13459, req_proto.GetPort());
    EXPECT_EQ("/csutil/test.html", req_proto.GetRelativeUrl());
    EXPECT_EQ("Name=LiSi&Age=11&Height=1.1", req_proto.GetGetParams());
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

    // HttpReqProtoFunc
    int real_len = 0;
    ret = HttpReqProtoFunc(stream_data.data(), stream_data.size(), &real_len);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(stream_data.size(), real_len);

    // Http Decode
    HttpProto req_proto;
    ret = req_proto.DecodeFromReq(stream_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(base::POST, req_proto.GetHttpType());
    EXPECT_EQ("HTTP/1.1", req_proto.GetProtoVersion());
    EXPECT_EQ("1.1.1.1", req_proto.GetHost());
    EXPECT_EQ(13459, req_proto.GetPort());
    EXPECT_EQ("/csutil/test.html", req_proto.GetRelativeUrl());
    EXPECT_EQ("Name=LiSi&Age=11&Height=1.1", req_proto.GetGetParams());
    EXPECT_EQ(post_params, req_proto.GetPostParams());
}/*}}}*/

TEST(HttpProto, Test_Normal_EncodeResp)
{/*{{{*/
    using namespace base;
    HttpProto http_proto;

    int ret_code = 200;
    std::string user_data = "<html> <body> AAA </body> </html>";

    // Encode HttpResponse
    std::string stream_data;
    Code ret = http_proto.EncodeToResponse(ret_code, user_data, &stream_data);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", stream_data.c_str());

    // HttpRespProtoFunc
    int real_len = 0;
    ret = HttpRespProtoFunc(stream_data.data(), (int)stream_data.size(), &real_len);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(stream_data.size(), real_len);

    // Decode HttpResponse
    HttpProto resp_proto;
    std::string rsp_user_data;
    ret = resp_proto.DecodeFromResponse(stream_data, &rsp_user_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(user_data, rsp_user_data);
    fprintf(stderr, "user_data:%s\n", rsp_user_data.c_str());
}/*}}}*/

TEST(HttpProto, Test_Normal_EncodeResp_EmptyContent)
{/*{{{*/
    using namespace base;
    HttpProto http_proto;

    int ret_code = 200;
    std::string user_data = "";

    // Encode HttpResponse
    std::string stream_data;
    Code ret = http_proto.EncodeToResponse(ret_code, user_data, &stream_data);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", stream_data.c_str());

    // HttpRespProtoFunc
    int real_len = 0;
    ret = HttpRespProtoFunc(stream_data.data(), (int)stream_data.size(), &real_len);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(stream_data.size(), real_len);

    // Decode HttpResponse
    HttpProto resp_proto;
    std::string rsp_user_data;
    ret = resp_proto.DecodeFromResponse(stream_data, &rsp_user_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(user_data, rsp_user_data);
    fprintf(stderr, "user_data:%s\n", rsp_user_data.c_str());
}/*}}}*/

TEST(HttpProto, Test_Exception_EncodeReq_GET)
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

    // HttpReqProtoFunc
    int real_len = 0;
    ret = HttpReqProtoFunc(stream_data.data(), stream_data.size()-1, &real_len);
    EXPECT_EQ(kDataNotEnough, ret);
    ret = HttpReqProtoFunc(stream_data.data(), stream_data.size()+1, &real_len);
    EXPECT_EQ(stream_data.size(), real_len);

    HttpProto req_proto;
    ret = req_proto.DecodeFromReq(stream_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(base::GET, req_proto.GetHttpType());
    EXPECT_EQ("HTTP/1.1", req_proto.GetProtoVersion());
    EXPECT_EQ("1.1.1.1", req_proto.GetHost());
    EXPECT_EQ(13459, req_proto.GetPort());
    EXPECT_EQ("/csutil/test.html", req_proto.GetRelativeUrl());
    EXPECT_EQ("Name=LiSi&Age=11&Height=1.1", req_proto.GetGetParams());
}/*}}}*/

TEST(HttpProto, Test_Exception_EncodeResp)
{/*{{{*/
    using namespace base;
    HttpProto http_proto;

    int ret_code = 200;
    std::string user_data = "<html> <body> AAA </body> </html>";

    // Encode HttpResponse
    std::string stream_data;
    Code ret = http_proto.EncodeToResponse(ret_code, user_data, &stream_data);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "%s\n", stream_data.c_str());

    // HttpRespProtoFunc
    int real_len = 0;
    ret = HttpRespProtoFunc(stream_data.data(), (int)stream_data.size()-1, &real_len);
    EXPECT_EQ(kDataNotEnough, ret);
    ret = HttpRespProtoFunc(stream_data.data(), (int)stream_data.size()+1, &real_len);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(stream_data.size(), real_len);

    // Decode HttpResponse
    HttpProto resp_proto;
    std::string rsp_user_data;
    ret = resp_proto.DecodeFromResponse(stream_data, &rsp_user_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(user_data, rsp_user_data);
    fprintf(stderr, "user_data:%s\n", rsp_user_data.c_str());
}/*}}}*/

TEST(HttpProto, Test_Exception_EncodeResp_NoContentLength)
{/*{{{*/
    using namespace base;
    std::string stream_data = "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/html; charset=UTF-8\r\n"
              "Connection: Keep-Alive\r\n"
              "\r\n";

    // Decode HttpResponse
    HttpProto resp_proto;
    std::string rsp_user_data;
    Code ret = resp_proto.DecodeFromResponse(stream_data, &rsp_user_data);
    EXPECT_EQ(kNoContentLength, ret);
}/*}}}*/

TEST(HttpProto, Test_Exception_EncodeReq_POST)
{/*{{{*/
    using namespace base;
    std::string url = "http://1.1.1.1:13459/csutil/test.html?Name=LiSi&Age=11&Height=1.1";
    HttpProto http_proto;
    Code ret = http_proto.SetHttpType(base::POST);
    EXPECT_EQ(kOk, ret);

    std::string get_stream_data;
    std::string host;
    uint16_t port = 0;
    ret = http_proto.EncodeToReq(url, &get_stream_data, &host, &port);
    EXPECT_EQ(kInvalidHttpType, ret);
}/*}}}*/
