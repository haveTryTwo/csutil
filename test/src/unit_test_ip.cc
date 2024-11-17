// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "base/coding.h"
#include "base/common.h"
#include "base/ip.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(GetUintIpByStr, Test_Normal_Ip) { /*{{{*/
  using namespace base;

  std::string str_ip = "35.111.186.7";
  uint32_t default_ip = 0x07ba6f23;

  uint32_t uint_ip = 0;
  Code ret = GetUIntIpByStr(str_ip, &uint_ip);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_ip, uint_ip);
  fprintf(stderr, "str_ip:%s, uint_ip:%#x\n", str_ip.c_str(), (unsigned int)uint_ip);
} /*}}}*/

TEST(GetStrIpByUint, Test_Normal_Ip) { /*{{{*/
  using namespace base;

  uint32_t ip = 0x07ba6f23;
  // uint32_t ip = 3514996490;
  // uint32_t ip = INADDR_ANY;
  // uint32_t ip = 2936926730;
  // uint32_t ip = 693318154;
  // uint32_t ip = 1747070473;
  std::string default_str_ip = "35.111.186.7";

  std::string str_ip;
  Code ret = GetStrIpByUint(ip, &str_ip);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_str_ip, str_ip);
  fprintf(stderr, "str_ip:%s, uint_ip:%#x\n", str_ip.c_str(), (unsigned int)ip);
} /*}}}*/
