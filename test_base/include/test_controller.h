// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_CONTROLLER_H_
#define TEST_BASE_TEST_CONTROLLER_H_

#include <string>
#include <vector>

#include "strategy/singleton.h"

#include "test_base/include/test_base.h"

namespace test {
class Test;

class TestController { /*{{{*/
 public:
  TestController();
  ~TestController();

 public:
  void Run();
  void Register(Test *test);

 private:
  void ClearTestCases();

 private:
  std::vector<std::pair<std::string, std::vector<Test *> > > test_cases_;

  int test_case_num_;
  int test_num_;
  int succ_test_num_;
  int fail_test_num_;

  std::vector<std::string> fail_tests_;
}; /*}}}*/

}  // namespace test

#endif
