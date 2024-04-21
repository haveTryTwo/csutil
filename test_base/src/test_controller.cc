// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/time.h>

#include "base/common.h"
#include "base/file_util.h"

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

      if (real_test->GetIsDataDriven()) {
        RunDataCases(real_test);
      } else {
        real_test->ExecBody();
      }

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

void TestController::RunDataCases(Test *test) { /*{{{*/
  // NOTE:htt, read data-driven test case files
  std::string cnt;
  base::Code ret = base::PumpWholeData(test->GetDataDrivenPath(), &cnt);
  EXPECT_TEST_EQ(base::kOk, ret, test);
  rapidjson::Document d;
  d.Parse(cnt.c_str(), cnt.size());
  EXPECT_TEST_EQ(false, d.HasParseError(), test);
  EXPECT_TEST_EQ(true, d.IsObject(), test);
  EXPECT_TEST_EQ(true, d.HasMember(base::kTestCases), test);
  EXPECT_TEST_EQ(true, d[base::kTestCases].IsArray(), test);

  struct timeval data_test_begin;
  gettimeofday(&data_test_begin, NULL);
  PrintDataTestCaseInfoBeforeRun(test, d[base::kTestCases].Size());

  rapidjson::Value::ConstValueIterator it = d[base::kTestCases].Begin();
  for (; it != d[base::kTestCases].End(); ++it) {
    EXPECT_TEST_EQ(true, it->IsObject(), test);
    // NOTE:htt, 数据驱动用例名称
    EXPECT_TEST_EQ(true, it->HasMember(base::kCaseName), test);
    EXPECT_TEST_EQ(true, (*it)[base::kCaseName].IsString(), test);
    EXPECT_TEST_EQ(true, it->HasMember(base::kCaseDesc), test);
    EXPECT_TEST_EQ(true, (*it)[base::kCaseDesc].IsString(), test);

    test->SetIsDataDrivenSucc(true);  // NOTE:htt, 恢复默认值

    PrintDataTestInfoBeforeRun((*it)[base::kCaseName].GetString(),
                               (*it)[base::kCaseDesc].GetString());
    struct timeval test_begin;
    gettimeofday(&test_begin, NULL);

    test->ExecBody(*it);

    struct timeval test_end;
    gettimeofday(&test_end, NULL);
    PrintDataTestInfoAfterRun((*it)[base::kCaseName].GetString(), test_begin, test_end,
                              test->GetIsDataDrivenSucc());
  }

  struct timeval data_test_end;
  gettimeofday(&data_test_end, NULL);
  PrintDataTestCaseInfoAfterRun(test, d[base::kTestCases].Size(), data_test_begin, data_test_end);
} /*}}}*/

void TestController::PrintDataTestInfoBeforeRun(const std::string &case_name,
                                                const std::string &case_desc) { /*{{{*/
  fprintf(stderr, "\033[35;1m[ RUN      ]\033[0m %s (测试目标:%s)\n", case_name.c_str(),
          case_desc.c_str());
} /*}}}*/

void TestController::PrintDataTestInfoAfterRun(const std::string &case_name,
                                               const struct timeval &begin_time,
                                               const struct timeval &end_time,
                                               bool is_data_driven_succ) { /*{{{*/
  int64_t diff_time = (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds +
                      (end_time.tv_usec - begin_time.tv_usec);

  if (is_data_driven_succ) {
    fprintf(stderr, "\033[35;1m[      OK  ]\033[0m %s (%lld s, %lld μs)\n", case_name.c_str(),
            (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);
  } else {
    fprintf(stderr, "\033[31;1m[     FAIL ]\033[0m %s (%lld s, %lld μs)\n", case_name.c_str(),
            (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);
  }
} /*}}}*/

void TestController::PrintDataTestCaseInfoBeforeRun(const Test *test,
                                                    uint32_t data_cases_size) { /*{{{*/
  fprintf(stderr, "\033[36;1m\n[----------]\033[0m %zu data test cases for %s\n",
          (unsigned int)data_cases_size,
          (test->GetTestCaseName() + "." + test->GetTestName()).c_str());
} /*}}}*/

void TestController::PrintDataTestCaseInfoAfterRun(const Test *test, uint32_t data_cases_size,
                                                   const struct timeval &begin_time,
                                                   const struct timeval &end_time) { /*{{{*/
  int64_t diff_case_time = (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds +
                           (end_time.tv_usec - begin_time.tv_usec);

  fprintf(
      stderr,
      "\033[36;1m[----------]\033[0m %zu data test cases for %s (test total %lld s, %lld μs)\n\n",
      (unsigned int)data_cases_size, (test->GetTestCaseName() + "." + test->GetTestName()).c_str(),
      (long long int)diff_case_time / base::kUnitConvOfMicrosconds,
      (long long int)diff_case_time % base::kUnitConvOfMicrosconds);
} /*}}}*/

}  // namespace test

int main(int argc, char *argv[]) { /*{{{*/
  strategy::Singleton<test::TestController>::Instance()->Run();

  // NOTE: this is last thing!
  strategy::Singleton<test::TestController>::DestroyInstance();

  return 0;
} /*}}}*/
