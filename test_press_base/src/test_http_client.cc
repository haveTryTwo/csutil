// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <deque>
#include <string>

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base/common.h"
#include "base/event.h"
#include "base/log.h"
#include "base/mutex.h"
#include "base/util.h"

#include "test_press_base/include/test_http_client.h"
#include "test_press_base/include/test_press_base.h"

namespace test {
RegisterBusiClient(http, HttpBusiClient);

HttpBusiClient::HttpBusiClient(const std::string &client_name)
    : BusiClient(client_name), is_init_(false), http_type_(base::GET) { /*{{{*/
} /*}}}*/

HttpBusiClient::~HttpBusiClient() { /*{{{*/
} /*}}}*/

base::Code HttpBusiClient::Init(const std::string &dst_ip_port_proto) { /*{{{*/
  std::deque<std::string> ip_port_proto;
  base::Code ret = base::Strtok(dst_ip_port_proto, base::kColon, &ip_port_proto);
  if (ret != base::kOk) return ret;
  if (ip_port_proto.size() < 3 || ip_port_proto.size() > 4) return base::kInvalidParam;
  if (ip_port_proto[2] != kHttpStr) return base::kInvalidParam;

  dst_ip_ = ip_port_proto[0];
  dst_port_ = (uint16_t)atoi(ip_port_proto[1].c_str());
  dst_ip_port_ = ip_port_proto[0] + ":" + ip_port_proto[1];

  if (ip_port_proto.size() == 4) {
    if (ip_port_proto[3] == kHttpGet) {
      http_type_ = base::GET;
    } else if (ip_port_proto[3] == kHttpPost) {
      http_type_ = base::POST;
    } else {
      return base::kInvalidParam;
    }
  } else {
    http_type_ = base::GET;  // Note: Default to GET for HTTP
  }

  ret = http_client_.Init();
  if (ret != base::kOk) return ret;

  is_init_ = true;

  return base::kOk;
} /*}}}*/

BusiClient *HttpBusiClient::Create() { /*{{{*/
  return new HttpBusiClient(*this);
} /*}}}*/

base::Code HttpBusiClient::SendAndRecv(const std::string &req_relative_url,
                                       const std::string &post_params, std::string *resp) { /*{{{*/
  if (resp == NULL) return base::kInvalidParam;
  if (!is_init_) return base::kNotInit;
  resp->clear();

  base::Code ret = base::kOk;
  char buf[128] = {0};
  if (req_relative_url.empty() || req_relative_url == "/") {
    snprintf(buf, sizeof(buf) - 1, "http://%s:%u", dst_ip_.c_str(), (unsigned)dst_port_);
  } else {
    snprintf(buf, sizeof(buf) - 1, "http://%s:%u/%s", dst_ip_.c_str(), (unsigned)dst_port_,
             req_relative_url.c_str());
  }

  if (http_type_ == base::GET) {
    ret = http_client_.Get(buf, resp);
    if (ret != base::kOk) return ret;
  } else if (http_type_ == base::POST) {
    ret = http_client_.Post(buf, post_params, resp);
    if (ret != base::kOk) return ret;
  } else {
    return base::kInvalidParam;
  }

  return ret;
} /*}}}*/

}  // namespace test
