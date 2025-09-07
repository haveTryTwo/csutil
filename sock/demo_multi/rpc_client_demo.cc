// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

#include <assert.h>

#include <string>

#ifdef _TCP_CLIENT_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string ip("127.0.0.1");
  uint16_t port = 9001;
  fprintf(stderr, "Start to connect ip:port <%s, %d>\n", ip.c_str(), port);

  RpcClient rpc_client(ip, port);
  base::EventType event_type = base::kPoll;
#  if defined(__linux__)
  event_type = base::kEPoll;
#  endif
  Code ret = rpc_client.Init(event_type, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc);
  assert(ret == base::kOk);

  for (int i = 0; i < 1; ++i) {
    std::string request("[check]");
    request += std::to_string(i);
    std::string response;
    ret = rpc_client.SendAndRecv(request, &response);
    assert(ret == kOk);
    fprintf(stderr, "request:%s, response:%s\n", request.c_str(), response.c_str());
  }

//  for (int i = 0; i < 1; ++i) {
//    std::string request("hello world");
//    request += std::to_string(i);
//    std::string response;
//    ret = rpc_client.SendAndRecv(request, &response);
//    assert(ret == kOk);
//    fprintf(stderr, "request:%s, response:%s\n", request.c_str(), response.c_str());
//  }

  return 0;
} /*}}}*/
#endif
