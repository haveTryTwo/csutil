// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_PRESS_BASE_H_
#define TEST_PRESS_BASE_H_

#include <map>
#include <string>
#include <vector>

#include <stdint.h>

#include "base/status.h"
#include "base/time.h"
#include "strategy/singleton.h"

#include "test_press_base/include/test_busi_client.h"

namespace test {
struct ResultInfo {
  ResultInfo() : total_num(0), succ_num(0), fail_num(0), max_req_ms(0), total_time_us(0) {}

  uint32_t total_num;
  uint32_t succ_num;
  uint32_t fail_num;
  uint32_t max_req_ms;
  uint64_t total_time_us;
};

class PressObject {
 public:
  PressObject(const std::string &test_name);
  virtual ~PressObject();

 public:
  // Constructing subclass objects
  virtual PressObject *Create() = 0;

  // dst_ip_port_protos may contain more than one
  // dst_ip_port_proto: 1.1.1.1:80:http:GET,2.2.2.2:90:rpc
  virtual base::Code Init(const std::string &dst_ip_port_protos);
  virtual base::Code ExecBody();

  // The subclass provides stress test code end judgment, for example, you can define a member variable to implement
  virtual bool IsOver();

 public:
  base::Code Exec(ResultInfo *res_info);
  std::string GetTestName();
  BusiClient *GetBusiClient(const std::string &proto_name);
  base::Code SetThreadIndex(int thread_index);
  int GetThreadIndex();

 protected:
  uint32_t r_seed_;
  std::string dst_ip_port_protos_;
  std::string test_name_;
  std::map<std::string, std::vector<BusiClient *> > busi_clients_;
  int thread_index_;
};

}  // namespace test

test::PressObject *MakeRegister(const std::string &test_name, test::PressObject *test_obj);

#define Register(test_name, class_name) \
  test::PressObject *test_name##class_name = MakeRegister(#test_name, new class_name(#test_name))

test::BusiClient *MakeBusiClientRegister(const std::string &client_name, test::BusiClient *busi_client);

#define RegisterBusiClient(client_name, class_name) \
  test::BusiClient *client_name##class_name = MakeBusiClientRegister(#client_name, new class_name(#client_name))

#endif
