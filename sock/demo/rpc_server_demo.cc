// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_server.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/daemon.h"
#include "base/status.h"
#include "sock/demo_multi/proto/demo_multi.pb.h"

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

/**
 * @brief Echo 风格的 PbRpcAction，将请求消息原样复制到响应
 * @param conf 用户配置
 * @param req 请求 protobuf 消息指针
 * @param resp 响应 protobuf 消息指针
 * @return kOk 成功
 */
base::Code EchoPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
                           ::google::protobuf::Message *resp) { /*{{{*/
  if (req == NULL || resp == NULL) return base::kInvalidParam;

  const demo_multi::ClientReq *client_req = dynamic_cast<const demo_multi::ClientReq *>(req);
  demo_multi::ClientResp *client_resp = dynamic_cast<demo_multi::ClientResp *>(resp);
  if (client_req == NULL || client_resp == NULL) return base::kInvalidParam;

  demo_multi::BaseResp *base_resp = client_resp->mutable_base();
  base_resp->set_ret_code(0);
  base_resp->set_ret_msg("success");

  std::string prefix_key;
  conf.GetValue("prefix_key", &prefix_key);
  client_resp->set_message(prefix_key + client_req->message());

  return base::kOk;
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string conf_path = "./conf/server.conf";
  if (argc == 1) {
    fprintf(stderr, "config path using default: %s\n\n", conf_path.c_str());
  } else {
    int32_t opt = 0;
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
      switch (opt) {
        case 's':
          conf_path = optarg;
          break;
        case 'h':
          Help(argv[0]);
          return 0;
        default:
          fprintf(stderr, "Not right options\n");
          Help(argv[0]);
          return -1;
      }
    }
  }

  Config config;
  Code ret = config.LoadFile(conf_path);
  if (ret != kOk) {
    fprintf(stderr, "Failed to load conf:%d, then use no config\n", ret);
  }

  int is_daemon = 0;
  ret = config.GetInt32Value(kDaemonKey, &is_daemon);
  if (ret != kOk) {
    fprintf(stderr, "Failed to get daemon conf:%d, then use no daemon\n", ret);
  }

  if (is_daemon == 1) {
    fprintf(stderr, "Keep daemon\n");
    ret = DaemonAndKeepAlive();
    if (ret != kOk) return EXIT_FAILURE;
  } else {
    fprintf(stderr, "Keep not daemon\n");
  }

  std::string user_conf_path = "./conf/user_info.conf";
  Config user_conf;
  ret = user_conf.LoadFile(user_conf_path);
  if (ret != kOk) {
    LOG_ERR("Failed to load user conf:%s, ret:%d\n", user_conf_path.c_str(), ret);
    return ret;
  }

  demo_multi::ClientReq req_prototype;
  demo_multi::ClientResp resp_prototype;

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   EchoPbRpcAction, &req_prototype, &resp_prototype);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
