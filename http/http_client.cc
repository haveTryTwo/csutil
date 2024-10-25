// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "http/http_client.h"
#include "base/event.h"
#include "base/ip.h"
#include "base/log.h"

namespace base {

HttpClient::HttpClient() : tcp_client_(), http_proto_() {}

HttpClient::~HttpClient() {}

Code HttpClient::Init() { /*{{{*/
  EventType event_type = kPoll;
#if defined(__linux__)
  event_type = kEPoll;
#endif
  Code ret = tcp_client_.Init(event_type, HttpRespProtoFunc);
  if (ret != kOk) return ret;

  //    ret = http_proto_.SetHttpType(POST);
  //    if (ret != kOk) return ret;

  return ret;
} /*}}}*/

Code HttpClient::Perform(const std::string &url, const std::string &params, std::string *result,
                         HttpType http_type /*=GET*/) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  Code ret = http_proto_.SetHttpType(http_type);
  if (ret != kOk) return ret;

  std::string host;
  uint16_t port;
  std::string stream_data_req;
  switch (http_type) {
    case GET:
      ret = http_proto_.EncodeToReq(url, &stream_data_req, &host, &port);
      if (ret != kOk) return ret;
      break;
    case POST:
      ret = http_proto_.EncodeToReq(url, params, &stream_data_req, &host, &port);
      if (ret != kOk) return ret;
      break;
    default:
      return kInvalidHttpType;
  }

  //  LOG_ERR("stream_data_req:%s", stream_data_req.c_str());

  std::string ip;
  ret = GetHostIpByName(host, &ip);
  if (ret != kOk) return ret;

  ret = tcp_client_.Connect(ip, port);
  if (ret != kOk) return ret;

  ret = tcp_client_.SendNative(stream_data_req);
  if (ret != kOk) return ret;

  std::string stream_data_resp;
  ret = tcp_client_.Recv(&stream_data_resp);
  if (ret != kOk) return ret;

  ret = http_proto_.DecodeFromResponse(stream_data_resp, result);
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

Code HttpClient::Post(const std::string &url, const std::string &post_params, std::string *result) { /*{{{*/
  return Perform(url, post_params, result, POST);
} /*}}}*/

Code HttpClient::Get(const std::string &url, std::string *result) { /*{{{*/
  return Perform(url, "", result, GET);
} /*}}}*/

Code HttpClient::SetUserNameAndPwd(const std::string &user_name, const std::string &pwd) { /*{{{*/
  return http_proto_.SetUserNameAndPwd(user_name, pwd);
} /*}}}*/

Code HttpClient::ClearUserNameAndPwd() { /*{{{*/ return http_proto_.ClearUserNameAndPwd(); } /*}}}*/

}  // namespace base

#ifdef _HTTP_CLIENT_MAIN_TEST_
#  include <stdio.h>

int main(int argc, char *argv[]) {
  using namespace base;

  HttpClient http_client;
  Code ret = http_client.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to Http Client init! ret:%d", ret);
    return ret;
  }

  //    std::string url = "http://translate.google.cn/";
  //    std::string url = "http://www.google.com.hk/";
  //    std::string url = "http://translate.google.cn/#en/zh-CN/peek";
  std::string url = "http://mirrors.ustc.edu.cn/gnu/bash/";
  std::string result;

  ret = http_client.Perform(url, "", &result, GET);
  if (ret != kOk) {
    LOG_ERR("Failed to perform url:%s, ret:%d", url.c_str(), ret);
    return ret;
  }

  //    fprintf(stderr, "%s\n", result.c_str());

  return 0;
}
#endif
