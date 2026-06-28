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
#include "sock/demo_multi/proto/demo_multi.pb.h"

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

/**
 * @brief Control 服务的 protobuf RPC 处理函数，聚合 ACL 和 Study 两个后端服务
 * @param conf 用户配置
 * @param req ControlReq protobuf 请求
 * @param resp ControlResp protobuf 响应
 * @return kOk 成功
 */
base::Code ControlPbRpcAction(const base::Config &conf, const ::google::protobuf::Message *req,
                              ::google::protobuf::Message *resp) { /*{{{*/
  if (req == NULL || resp == NULL) return base::kInvalidParam;

  const demo_multi::ControlReq *ctrl_req = dynamic_cast<const demo_multi::ControlReq *>(req);
  demo_multi::ControlResp *ctrl_resp = dynamic_cast<demo_multi::ControlResp *>(resp);
  if (ctrl_req == NULL || ctrl_resp == NULL) return base::kInvalidParam;

  demo_multi::BaseResp *base_resp = ctrl_resp->mutable_base();
  base_resp->set_ret_code(0);
  base_resp->set_ret_msg("success");

  std::string acl_server_ip;
  std::string acl_server_port;
  std::string study_server_ip;
  std::string study_server_port;
  base::Code ret = conf.GetValue(kAclServerIp, &acl_server_ip);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("get acl_server_ip error");
    return base::kOk;
  }
  ret = conf.GetValue(kAclServerPort, &acl_server_port);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("get acl_server_port error");
    return base::kOk;
  }
  ret = conf.GetValue(kStudyServerIp, &study_server_ip);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("get study_server_ip error");
    return base::kOk;
  }
  ret = conf.GetValue(kStudyServerPort, &study_server_port);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("get study_server_port error");
    return base::kOk;
  }

  // 调用 ACL 后端服务
  base::RpcClient rpc_acl_client(acl_server_ip, atoi(acl_server_port.c_str()));
  ret = rpc_acl_client.Init();
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("connect acl server error");
    return base::kOk;
  }

  demo_multi::AclReq acl_req;
  acl_req.set_user_name(ctrl_req->target());
  acl_req.set_resource(ctrl_req->command());

  demo_multi::AclResp acl_resp;
  ret = rpc_acl_client.SendAndRecv(acl_req, &acl_resp);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("send to acl server error");
    return base::kOk;
  }
  ctrl_resp->set_acl_result(acl_resp.detail());

  // 调用 Study 后端服务
  base::RpcClient rpc_study_client(study_server_ip, atoi(study_server_port.c_str()));
  ret = rpc_study_client.Init();
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("connect study server error");
    return base::kOk;
  }

  demo_multi::StudyReq study_req;
  study_req.set_student_name(ctrl_req->target());
  study_req.set_subject(ctrl_req->command());

  demo_multi::StudyResp study_resp;
  ret = rpc_study_client.SendAndRecv(study_req, &study_resp);
  if (ret != base::kOk) {
    base_resp->set_ret_code(ret);
    base_resp->set_ret_msg("send to study server error");
    return base::kOk;
  }
  ctrl_resp->set_study_result(study_resp.detail());

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

  demo_multi::ControlReq req_prototype;
  demo_multi::ControlResp resp_prototype;

  RpcServer server(config, user_conf, DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
                   ControlPbRpcAction, &req_prototype, &resp_prototype);
  ret = server.Init();
  if (ret != kOk) {
    LOG_ERR("Failed to init RpcServer, ret:%d\n", ret);
  }
  assert(ret == kOk);

  ret = server.Run();

  return ret;
} /*}}}*/
