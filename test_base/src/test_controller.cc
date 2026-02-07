// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/time.h>

#include "base/common.h"
#include "base/file_util.h"
#include "base/status.h"

#include "test_base/include/test_controller.h"

namespace test {

TestController::TestController()
    : test_case_num_(0),
      test_num_(0),
      succ_test_num_(0),
      fail_test_num_(0),
      filter_pattern_(""),
      actual_test_case_num_(0),
      actual_test_num_(0) { /*{{{*/ } /*}}}*/

TestController::~TestController() { /*{{{*/ ClearTestCases(); } /*}}}*/

void TestController::Run() { /*{{{*/
  // 重置实际执行的统计
  actual_test_case_num_ = 0;
  actual_test_num_ = 0;

  PrintTestCasesInfoBeforeRun();
  struct timeval all_cases_begin;
  gettimeofday(&all_cases_begin, NULL);

  std::vector<std::pair<std::string, std::vector<Test *>>>::iterator it = test_cases_.begin();
  for (; it != test_cases_.end(); ++it) { /*{{{*/
    // 统计该 TestCase 中实际执行的测试数量
    int actual_tests_in_case = 0;
    struct timeval test_case_begin;
    gettimeofday(&test_case_begin, NULL);

    std::vector<Test *>::iterator test_it = it->second.begin();
    for (; test_it != it->second.end(); ++test_it) { /*{{{*/
      Test *real_test = *test_it;

      // 检查是否应该运行此测试（过滤功能）
      if (!ShouldRunTest(real_test->GetTestCaseName(), real_test->GetTestName())) {
        continue;  // 跳过不匹配的测试
      }

      // 如果是该 TestCase 中第一个执行的测试，打印 TestCase 信息
      if (actual_tests_in_case == 0) {
        PrintTestCaseInfoBeforeRun(*it);
        ++actual_test_case_num_;
      }

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

      ++actual_tests_in_case;
      ++actual_test_num_;
    } /*}}}*/

    // 只有当该 TestCase 中有实际执行的测试时，才打印 TestCase 结束信息
    if (actual_tests_in_case > 0) {
      struct timeval test_case_end;
      gettimeofday(&test_case_end, NULL);
      PrintTestCaseInfoAfterRun(*it, test_case_begin, test_case_end, actual_tests_in_case);
    }
  } /*}}}*/

  struct timeval all_cases_end;
  gettimeofday(&all_cases_end, NULL);
  PrintTestCasesInfoAfterRun(all_cases_begin, all_cases_end);
} /*}}}*/

void TestController::Register(Test *test) { /*{{{*/
  std::vector<std::pair<std::string, std::vector<Test *>>>::iterator it = test_cases_.begin();
  for (; it != test_cases_.end(); ++it) {
    if (it->first == test->GetTestCaseName()) {
      break;
    }
  }

  if (it != test_cases_.end()) {
    it->second.push_back(test);
    ++test_num_;
  } else {
    test_cases_.push_back(std::pair<std::string, std::vector<Test *>>(test->GetTestCaseName(), std::vector<Test *>()));
    ++test_case_num_;

    std::vector<std::pair<std::string, std::vector<Test *>>>::reverse_iterator rit = test_cases_.rbegin();
    rit->second.push_back(test);
    ++test_num_;
  }
} /*}}}*/

void TestController::ClearTestCases() { /*{{{*/
  std::vector<std::pair<std::string, std::vector<Test *>>>::iterator it = test_cases_.begin();
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
    fprintf(stderr, "\033[32;1m[ RUN      ]\033[0m %s.%s (测试目标:%s)\n", test.GetTestCaseName().c_str(),
            test.GetTestName().c_str(), test.GetDesc().c_str());
  } else {
    fprintf(stderr, "\033[32;1m[ RUN      ]\033[0m %s.%s\n", test.GetTestCaseName().c_str(),
            test.GetTestName().c_str());
  }
} /*}}}*/

void TestController::PrintTestInfoAfterRun(const Test &test, const struct timeval &begin_time,
                                           const struct timeval &end_time) { /*{{{*/
  int64_t diff_time =
      (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds + (end_time.tv_usec - begin_time.tv_usec);

  if (test.GetIsSucc()) {
    fprintf(stderr, "\033[32;1m[      OK  ]\033[0m %s.%s (%lld s, %lld μs)\n", test.GetTestCaseName().c_str(),
            test.GetTestName().c_str(), (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);
    ++succ_test_num_;
  } else {
    fprintf(stderr, "\033[31;1m[     FAIL ]\033[0m %s.%s (%lld s, %lld μs)\n", test.GetTestCaseName().c_str(),
            test.GetTestName().c_str(), (long long int)diff_time / base::kUnitConvOfMicrosconds,
            (long long int)diff_time % base::kUnitConvOfMicrosconds);

    ++fail_test_num_;

    std::string fail_name = test.GetTestCaseName() + "." + test.GetTestName();
    fail_tests_.push_back(fail_name);
  }
} /*}}}*/

void TestController::PrintTestCaseInfoBeforeRun(const std::pair<std::string, std::vector<Test *>> &test_case) { /*{{{*/
  // 计算该 TestCase 中实际会执行的测试数量
  int actual_count = 0;
  std::vector<Test *>::const_iterator test_it = test_case.second.begin();
  for (; test_it != test_case.second.end(); ++test_it) {
    if (ShouldRunTest((*test_it)->GetTestCaseName(), (*test_it)->GetTestName())) {
      ++actual_count;
    }
  }
  fprintf(stderr, "\033[32;1m\n[----------]\033[0m %d tests for %s\n", actual_count, test_case.first.c_str());
} /*}}}*/

void TestController::PrintTestCaseInfoAfterRun(const std::pair<std::string, std::vector<Test *>> &test_case,
                                               const struct timeval &begin_time, const struct timeval &end_time,
                                               int actual_tests_count) { /*{{{*/
  int64_t diff_case_time =
      (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds + (end_time.tv_usec - begin_time.tv_usec);

  fprintf(stderr,
          "\033[32;1m[----------]\033[0m %d tests for %s (case total %lld s, "
          "%lld μs)\n\n",
          actual_tests_count, test_case.first.c_str(), (long long int)diff_case_time / base::kUnitConvOfMicrosconds,
          (long long int)diff_case_time % base::kUnitConvOfMicrosconds);
} /*}}}*/

void TestController::PrintTestCasesInfoBeforeRun() { /*{{{*/
  if (!filter_pattern_.empty()) {
    fprintf(stderr, "\033[33;1m[----------]\033[0m Note: Filtering tests by: %s\n", filter_pattern_.c_str());
  }
  // 如果有过滤模式，先不显示统计信息，等 Run() 结束后再显示实际执行的数字
  if (filter_pattern_.empty()) {
    fprintf(stderr, "\033[32;1m[==========]\033[0m Run %d tests in %d test cases\n", test_num_, test_case_num_);
  }
} /*}}}*/

void TestController::PrintTestCasesInfoAfterRun(const struct timeval &begin_time,
                                                const struct timeval &end_time) { /*{{{*/
  int64_t diff_all_time =
      (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds + (end_time.tv_usec - begin_time.tv_usec);

  // 如果有过滤模式，使用实际执行的数字；否则使用注册的数字
  int display_test_num = filter_pattern_.empty() ? test_num_ : actual_test_num_;
  int display_test_case_num = filter_pattern_.empty() ? test_case_num_ : actual_test_case_num_;

  fprintf(stderr,
          "\033[32;1m[==========]\033[0m Run %d tests in %d test cases (total "
          "%lld s, %lld μs)\n",
          display_test_num, display_test_case_num, (long long int)diff_all_time / base::kUnitConvOfMicrosconds,
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

    PrintDataTestInfoBeforeRun((*it)[base::kCaseName].GetString(), (*it)[base::kCaseDesc].GetString());
    struct timeval test_begin;
    gettimeofday(&test_begin, NULL);

    test->ExecBody(*it);

    struct timeval test_end;
    gettimeofday(&test_end, NULL);
    PrintDataTestInfoAfterRun((*it)[base::kCaseName].GetString(), test_begin, test_end, test->GetIsDataDrivenSucc());
  }

  struct timeval data_test_end;
  gettimeofday(&data_test_end, NULL);
  PrintDataTestCaseInfoAfterRun(test, d[base::kTestCases].Size(), data_test_begin, data_test_end);
} /*}}}*/

void TestController::PrintDataTestInfoBeforeRun(const std::string &case_name, const std::string &case_desc) { /*{{{*/
  fprintf(stderr, "\033[35;1m[ RUN      ]\033[0m %s (测试目标:%s)\n", case_name.c_str(), case_desc.c_str());
} /*}}}*/

void TestController::PrintDataTestInfoAfterRun(const std::string &case_name, const struct timeval &begin_time,
                                               const struct timeval &end_time, bool is_data_driven_succ) { /*{{{*/
  int64_t diff_time =
      (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds + (end_time.tv_usec - begin_time.tv_usec);

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

void TestController::PrintDataTestCaseInfoBeforeRun(const Test *test, uint32_t data_cases_size) { /*{{{*/
  fprintf(stderr, "\033[36;1m\n[----------]\033[0m %u data test cases for %s\n", (unsigned)data_cases_size,
          (test->GetTestCaseName() + "." + test->GetTestName()).c_str());
} /*}}}*/

void TestController::PrintDataTestCaseInfoAfterRun(const Test *test, uint32_t data_cases_size,
                                                   const struct timeval &begin_time,
                                                   const struct timeval &end_time) { /*{{{*/
  int64_t diff_case_time =
      (end_time.tv_sec - begin_time.tv_sec) * base::kUnitConvOfMicrosconds + (end_time.tv_usec - begin_time.tv_usec);

  fprintf(stderr,
          "\033[36;1m[----------]\033[0m %u data test cases for %s (test total "
          "%lld s, %lld μs)\n\n",
          (unsigned)data_cases_size, (test->GetTestCaseName() + "." + test->GetTestName()).c_str(),
          (long long int)diff_case_time / base::kUnitConvOfMicrosconds,
          (long long int)diff_case_time % base::kUnitConvOfMicrosconds);
} /*}}}*/

void TestController::ParseCommandLine(int argc, char *argv[]) { /*{{{*/
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    // 查找 --gtest_filter=
    const std::string prefix = "--gtest_filter=";
    if (arg.find(prefix) == 0) {
      filter_pattern_ = arg.substr(prefix.length());
      break;
    }
  }
} /*}}}*/

bool TestController::ShouldRunTest(const std::string &test_case_name, const std::string &test_name) const { /*{{{*/
  // 如果没有过滤器，运行所有测试
  if (filter_pattern_.empty()) {
    return true;
  }

  // 解析过滤模式
  std::string test_case_pattern, test_name_pattern;
  base::Code ret = ParseFilterPattern(filter_pattern_, &test_case_pattern, &test_name_pattern);
  if (ret != base::kOk) {
    // 解析失败，默认运行所有测试
    return true;
  }

  // 匹配测试用例名和测试名
  return MatchPattern(test_case_name, test_case_pattern) && MatchPattern(test_name, test_name_pattern);
} /*}}}*/

base::Code TestController::ParseFilterPattern(const std::string &pattern, std::string *test_case_pattern,
                                              std::string *test_name_pattern) const { /*{{{*/
  // 参数检查
  if (test_case_pattern == NULL || test_name_pattern == NULL) {
    return base::kInvalidParam;
  }

  // 清空输出参数
  test_case_pattern->clear();
  test_name_pattern->clear();

  // 检查模式是否为空
  if (pattern.empty()) {
    return base::kInvalidParam;
  }

  size_t dot_pos = pattern.find('.');
  if (dot_pos != std::string::npos) {
    // 检查点号位置是否有效
    if (dot_pos == 0 || dot_pos == pattern.length() - 1) {
      // 点号在开头或结尾，格式无效
      return base::kInvalidData;
    }
    *test_case_pattern = pattern.substr(0, dot_pos);
    *test_name_pattern = pattern.substr(dot_pos + 1);
  } else {
    // 如果没有点号，假设是测试用例过滤
    *test_case_pattern = pattern;
    *test_name_pattern = "*";  // 匹配所有测试名
  }

  return base::kOk;
} /*}}}*/

bool TestController::MatchPattern(const std::string &str, const std::string &pattern) const { /*{{{*/
  // 精确匹配
  if (pattern == str) {
    return true;
  }

  // 通配符 * 匹配任意
  if (pattern == "*") {
    return true;
  }

  // 简单的通配符匹配
  if (pattern.empty()) {
    return str.empty();
  }

  // *suffix 匹配（开头通配符）
  if (pattern.length() > 1 && pattern[0] == '*') {
    std::string suffix = pattern.substr(1);
    return str.length() >= suffix.length() && str.substr(str.length() - suffix.length()) == suffix;
  }

  // prefix* 匹配（结尾通配符）
  if (pattern.length() > 1 && pattern[pattern.length() - 1] == '*') {
    std::string prefix = pattern.substr(0, pattern.length() - 1);
    return str.length() >= prefix.length() && str.substr(0, prefix.length()) == prefix;
  }

  return false;
} /*}}}*/

}  // namespace test

int main(int argc, char *argv[]) { /*{{{*/
  // 解析命令行参数
  strategy::Singleton<test::TestController>::Instance()->ParseCommandLine(argc, argv);

  // 运行测试
  strategy::Singleton<test::TestController>::Instance()->Run();

  // NOTE: this is last thing!
  strategy::Singleton<test::TestController>::DestroyInstance();

  return 0;
} /*}}}*/
