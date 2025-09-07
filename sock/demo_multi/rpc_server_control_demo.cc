// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_server.h"
#include "sock/rpc_client.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include "base/daemon.h"
#include "base/status.h"

void Help(const std::string &program) { /*{{{*/
  fprintf(stderr,
          "Usage: %s [Option]\n"
          "  [-s conf_path]\n\n",
          program.c_str());
} /*}}}*/

const char kAclServerIp[] = "acl_server_ip";
const char kAclServerPort[] = "acl_server_port";
const char kStudyServerIp[] = "study_server_ip";
const char kStudyServerPort[] = "study_server_port";

base::Code ControlRpcAction(const base::Config &conf, const std::string &in, std::string *out) { /*{{{*/
  if (out == NULL) return base::kInvalidParam;

  out->assign(in + "\n");

  std::string acl_server_ip;
  std::string acl_server_port;
  std::string study_server_ip;
  std::string study_server_port;
  base::Code ret = conf.GetValue(kAclServerIp, &acl_server_ip);
  if (ret != base::kOk) {
    out->append("get ip or port error" + std::to_string(ret));
    return base::kOk;
  }
  ret = conf.GetValue(kAclServerPort, &acl_server_port);
  if (ret != base::kOk) {
    out->append("get ip or port error" + std::to_string(ret));
    return base::kOk;
  }
  ret = conf.GetValue(kStudyServerIp, &study_server_ip);
  if (ret != base::kOk) {
    out->append("get ip or port error" + std::to_string(ret));
    return base::kOk;
  }
  ret = conf.GetValue(kStudyServerPort, &study_server_port);
  if (ret != base::kOk) {
    out->append("get ip or port error" + std::to_string(ret));
    return base::kOk;
  }

  base::EventType event_type = base::kPoll;
#  if defined(__linux__)
  event_type = base::kEPoll;
#  endif
  base::RpcClient rpc_acl_client(acl_server_ip, atoi(acl_server_port.c_str()));
  ret = rpc_acl_client.Init(event_type, base::DefaultProtoFunc, base::DefaultGetUserDataFunc, base::DefaultFormatUserDataFunc);
  if (ret != base::kOk) {
    out->append("connect acl ip and port error" + std::to_string(ret));
    return base::kOk;
  }

  base::RpcClient rpc_study_client(study_server_ip, atoi(study_server_port.c_str()));
  ret = rpc_study_client.Init(event_type, base::DefaultProtoFunc, base::DefaultGetUserDataFunc, base::DefaultFormatUserDataFunc);
  if (ret != base::kOk) {
    out->append("connect study ip and port error" + std::to_string(ret));
    return base::kOk;
  }


  std::string response;
  ret = rpc_acl_client.SendAndRecv(in, &response);
  if (ret != base::kOk) {
    out->append("send ip and port error" + std::to_string(ret));
    return base::kOk;
  }
  out->append(response);

  response.clear();
  ret = rpc_study_client.SendAndRecv(in, &response);
  if (ret != base::kOk) {
    out->append("send ip and port error" + std::to_string(ret));
    return base::kOk;
  }
  out->append(response);

  return base::kOk;
} /*}}}*/

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string conf_path = "./conf/control_server.conf";
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

  std::string user_conf_path = "./conf/control_user_info.conf";
  Config user_conf;
  ret = user_conf.LoadFile(user_conf_path);
  if (ret != kOk) {
    LOG_ERR("Failed to load user conf:%s, ret:%d\n", user_conf_path.c_str(), ret);
    return ret;
  }

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   ControlRpcAction);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
