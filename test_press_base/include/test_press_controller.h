// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_CONTROLLER_H_
#define TEST_BASE_TEST_CONTROLLER_H_

#include <map>
#include <string>

#include "base/mutex.h"
#include "base/status.h"

#include "strategy/singleton.h"

#include "test_press_base/include/test_busi_client.h"
#include "test_press_base/include/test_press_base.h"

namespace test {
class PressObject;

class TestPressController {
 public:
  TestPressController();
  ~TestPressController();

 public:
  base::Code RegisterObject(PressObject *press_object);
  base::Code GetNewPressObject(const std::string &press_name, PressObject **press_object);

  base::Code RegisterClient(BusiClient *busi_client);
  base::Code GetNewBusiClient(const std::string &client_name, BusiClient **busi_client);

 private:
  void Clear();

 private:
  base::Mutex mu_;
  std::map<std::string, PressObject *> press_factory_;
  std::map<std::string, BusiClient *> client_factory_;
};

}  // namespace test

#endif
