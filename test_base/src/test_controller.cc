// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/time.h>

#include "base/common.h"

#include "test_base/include/test_controller.h"

namespace test {

TestController::TestController()
    : test_case_num_(0), test_num_(0), succ_test_num_(0), fail_test_num_(0) { /*{{{*/
} /*}}}*/

TestController::~TestController() { /*{{{*/
  ClearTestCases();
} /*}}}*/

void TestController::Run() { /*{{{*/
  PrintTestCasesInfoBeforeRun();

  struct timeval all_cases_begin;
  gettimeofday(&all_cases_begin, NULL);

  std::vector<std::pair<std::string, std::vector<Test *> > >::iterator it = test_cases_.begin();
  for (; it != test_cases_.end(); ++it) { /*{{{*/
    PrintTestCaseInfoBeforeRun(*it);

    struct timeval test_case_begin;
    gettimeofday(&test_case_begin, NULL);

    std::vector<Test *>::iterator test_it = it->second.begin();
    for (; test_it != it->second.end(); ++test_it) { /*{{{*/
      Test *real_test = *test_it;
      PrintTestInfoBeforeRun(*real_test);

      struct timeval test_begin;
      gettimeofday(&test_begin, NULL);

      real_test->Begin();

      real_test->ExecBody();

      struct timeval test_end;
      gettimeofday(&test_end, NULL);
      PrintTestInfoAfterRun(*real_test, test_begin, test_end);

      real_test->End();  // NOTE:htt, 将End()处理放在最后，保证处理的安全性
    }                    /*}}}*/

    struct timeval test_case_end;
    gettimeofday(&test_case_end, NULL);
    PrintTestCaseInfoAfterRun(*it, test_case_begin, test_case_end);
  } /*}}}*/

  struct timeval all_cases_end;
  gettimeofday(&all_cases_end, NULL);
  PrintTestCasesInfoAfterRun(all_cases_begin, all_cases_end);
} /*}}}*/

void TestController::Register(Test *test) { /*{{{*/
  std::vector<std::pair<std::string, std::vector<Test *> > >::iterator it = test_cases_.begin();
  for (; it != test_cases_.end(); ++it) {
    if (it->first == test->GetTestCaseName()) {
      break;
    }
  }

  if (it != test_cases_.end()) {
    it->second.push_back(test);
    ++test_num_;
  } else {
    test_cases_.push_back(std::pair<std::string, std::vector<Test *> >(test->GetTestCaseName(),
                                                                       std::vector<Test *>()));
    ++test_case_num_;

    std::vector<std::pair<std::string, std::vector<Test *> > >::reverse_iterator rit =
        test_cases_.rbegin();
    rit->second.push_back(test);
    ++test_num_;
  }
} /*}}}*/

void TestController::ClearTestCases() { /*{{{*/
  std::vector<std::pair<std::string, std::vector<Test *> > >::iterator it = test_cases_.begin();
  for (; it != test_cases_.end(); ++it) {
    std::vector<Test *>::iterator test_it = it->second.begin();
    for (; test_it != it->second.end(); ++test_it) {
      delete *test_it;
      *test_it = NULL;
    }
    it->second.clear();
  }
  test_cases_.clear();
} /*}}}*/

void TestController::PrintTestInfoBeforeRun(const Test &test) { /*{{{*/
  if (!test.GetDesc().empty()) {
    fprintf(stderr, "\033[32;1m[ RUN      ]\033[0m %s.%s (测试目标:%s)\n",
            test.GetTestCaseName().c_str(), test.GetTestName().c_str(), test.GetDesc().c_str());
  } else {
    fprintf(stderr, "\033[32;1m[ RUN      ]\033[0m %s.%s\n", test.GetTestCaseName().c_str(),
            test.GetTestName().c_str());
  }
} /*}}}*/

void TestController::PrintTestInfoAfterRun(const Test &test, const struct timeval &begin_time,
                                           const struct timeval &end_time) { /*{{{*/
  int64_t diff_time = (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds +
                      (end_time.tv_usec - begin_time.tv_usec);

  if (test.GetIsSucc()) {
    fprintf(stderr, "\033[32;1m[      OK  ]\033[0m %s.%s (%lld s, %lld μs)\n",
            test.GetTestCaseName().c_str(), test.GetTestName().c_str(),
            (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);
    ++succ_test_num_;
  } else {
    fprintf(stderr, "\033[31;1m[     FAIL ]\033[0m %s.%s (%lld s, %lld μs)\n",
            test.GetTestCaseName().c_str(), test.GetTestName().c_str(),
            (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);

    ++fail_test_num_;

    std::string fail_name = test.GetTestCaseName() + "." + test.GetTestName();
    fail_tests_.push_back(fail_name);
  }
} /*}}}*/

void TestController::PrintTestCaseInfoBeforeRun(
    const std::pair<std::string, std::vector<Test *> > &test_case) { /*{{{*/
  fprintf(stderr, "\033[32;1m\n[----------]\033[0m %zu tests for %s\n", test_case.second.size(),
          test_case.first.c_str());
} /*}}}*/

void TestController::PrintTestCaseInfoAfterRun(
    const std::pair<std::string, std::vector<Test *> > &test_case, const struct timeval &begin_time,
    const struct timeval &end_time) { /*{{{*/
  int64_t diff_case_time = (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds +
                           (end_time.tv_usec - begin_time.tv_usec);

  fprintf(stderr, "\033[32;1m[----------]\033[0m %zu tests for %s (case total %lld s, %lld μs)\n\n",
          test_case.second.size(), test_case.first.c_str(),
          (long long int)diff_case_time / base::kUnitConvOfMicrosconds,
          (long long int)diff_case_time % base::kUnitConvOfMicrosconds);
} /*}}}*/

void TestController::PrintTestCasesInfoBeforeRun() { /*{{{*/
  fprintf(stderr, "\033[32;1m[==========]\033[0m Run %d tests in %d test cases\n", test_num_,
          test_case_num_);
} /*}}}*/

void TestController::PrintTestCasesInfoAfterRun(const struct timeval &begin_time,
                                                const struct timeval &end_time) { /*{{{*/
  int64_t diff_all_time = (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds +
                          (end_time.tv_usec - begin_time.tv_usec);

  fprintf(stderr,
          "\033[32;1m[==========]\033[0m Run %d tests in %d test cases (total %lld s, %lld μs)\n",
          test_num_, test_case_num_, (long long int)diff_all_time / base::kUnitConvOfMicrosconds,
          (long long int)diff_all_time % base::kUnitConvOfMicrosconds);

  fprintf(stderr, "\033[32;1m[   PASS   ]\033[0m %d tests\n", succ_test_num_);
  fprintf(stderr, "\033[31;1m[   FAIL   ]\033[0m %d tests, as follow:\n", fail_test_num_);

  std::vector<std::string>::iterator fail_it = fail_tests_.begin();
  for (; fail_it != fail_tests_.end(); ++fail_it) {
    fprintf(stderr, "\033[31;1m[   FAIL   ]\033[0m %s\n", fail_it->c_str());
  }
} /*}}}*/

}  // namespace test

int main(int argc, char *argv[]) { /*{{{*/
  strategy::Singleton<test::TestController>::Instance()->Run();

  // NOTE: this is last thing!
  strategy::Singleton<test::TestController>::DestroyInstance();

  return 0;
} /*}}}*/
