// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/reg.h"

#include <stdint.h>

#include "base/common.h"
#include "base/log.h"

namespace base {

Reg::Reg(const std::string &regex) : regex_(regex) { preg_ = NULL; }

Reg::~Reg() { /*{{{*/
  if (preg_ != NULL) {
    regfree(preg_);

    delete (preg_);
    preg_ = NULL;
  }
} /*}}}*/

Code Reg::Init() { /*{{{*/
  preg_ = new regex_t;

  int ret = regcomp(preg_, regex_.c_str(), REG_EXTENDED | REG_NOSUB);
  if (ret != kOk) {
    char err_buf[kBufLen] = {0};
    regerror(ret, preg_, err_buf, sizeof(err_buf));
    LOG_ERR("Failed to regcomp! err:%s\n", err_buf);

    return kRegCompFailed;
  }

  return kOk;
} /*}}}*/

Code Reg::Check(const std::string &str) { /*{{{*/
  uint32_t nmatch = 1;
  regmatch_t pmatch[1];

  int ret = regexec(preg_, str.c_str(), nmatch, pmatch, 0);
  if (ret != 0) return kRegNotMatch;

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _REG_MAIN_TEST_
#  include <stdio.h>

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  // ip check
  std::string ip_reg = "^([0-9]{1,3}\\.){3}[0-9]{1,3}$";
  Reg reg(ip_reg);
  Code ret = reg.Init();
  if (ret != kOk) {
    fprintf(stderr, "Failed to init reg! ret:%d\n", ret);
    return ret;
  }

  std::string ip = "1.1.1.1";
  ret = reg.Check(ip);
  if (ret == kOk) {
    fprintf(stderr, "[ok], ip:%s\n", ip.c_str());
  } else {
    fprintf(stderr, "[Failed], ip:%s\n", ip.c_str());
  }

  // http check
  std::string http_reg = "^[hH][tT]{2}[pP]/[0-9]\\.[0-9] [0-9]{3} .*";
  Reg reg1(http_reg);
  ret = reg1.Init();
  if (ret != kOk) {
    fprintf(stderr, "Failed to init reg! ret:%d\n", ret);
    return ret;
  }

  std::string http = "HTTP/1.0 300 ok\r\n";
  ret = reg1.Check(http);
  if (ret == kOk) {
    fprintf(stderr, "[ok], %s\n", http.c_str());
  } else {
    fprintf(stderr, "[Failed], %s\n", http.c_str());
  }

  return 0;
} /*}}}*/
#endif
