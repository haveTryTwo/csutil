// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_client.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include "base/status.h"
#include "sock/demo_multi/proto/demo_multi.pb.h"

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-i ip] [-p port] [-n count]\n\n",
          program.c_str());
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string ip("127.0.0.1");
  uint16_t port = 9090;
  int32_t count = 4;

  int32_t opt = 0;
  while ((opt = getopt(argc, argv, "i:p:n:h")) != -1) {
    switch (opt) {
      case 'i':
        ip = optarg;
        break;
      case 'p':
        port = (uint16_t)atoi(optarg);
        break;
      case 'n':
        count = atoi(optarg);
        break;
      case 'h':
        Help(argv[0]);
        return 0;
      default:
        Help(argv[0]);
        return -1;
    }
  }

  fprintf(stderr, "Start to connect ip:port <%s, %d>, count:%d\n", ip.c_str(), port, count);

  RpcClient rpc_client(ip, port);
  Code ret = rpc_client.Init();
  assert(ret == kOk);

  for (int32_t i = 0; i < count; ++i) {
    demo_multi::ClientReq req;
    req.set_message("hello world " + std::to_string(i));
    req.set_index(i);

    demo_multi::ClientResp resp;
    ret = rpc_client.SendAndRecv(req, &resp);
    if (ret != kOk) {
      fprintf(stderr, "[%d] SendAndRecv failed, ret:%d\n", i, ret);
      continue;
    }

    fprintf(stderr, "[%d] ret_code:%d, ret_msg:%s, message:%s\n", i, resp.base().ret_code(),
            resp.base().ret_msg().c_str(), resp.message().c_str());
  }

  return 0;
} /*}}}*/
