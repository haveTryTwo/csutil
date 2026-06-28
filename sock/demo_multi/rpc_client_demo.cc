// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

#include <assert.h>

#include <string>

#include "sock/demo_multi/proto/demo_multi.pb.h"

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string ip("127.0.0.1");
  uint16_t port = 9001;
  fprintf(stderr, "Start to connect ip:port <%s, %d>\n", ip.c_str(), port);

  RpcClient rpc_client(ip, port);
  Code ret = rpc_client.Init();
  assert(ret == base::kOk);

  for (int i = 0; i < 2; ++i) {
    demo_multi::ControlReq request;
    request.set_command("check-" + std::to_string(i));
    request.set_target("user_" + std::to_string(i));

    demo_multi::ControlResp response;
    ret = rpc_client.SendAndRecv(request, &response);
    assert(ret == kOk);

    fprintf(stderr, "request: command=%s, target=%s\n", request.command().c_str(), request.target().c_str());
    fprintf(stderr, "response: ret_code=%d, ret_msg=%s\n", response.base().ret_code(),
            response.base().ret_msg().c_str());
    fprintf(stderr, "  acl_result: %s\n", response.acl_result().c_str());
    fprintf(stderr, "  study_result: %s\n\n", response.study_result().c_str());
  }

  return 0;
} /*}}}*/
