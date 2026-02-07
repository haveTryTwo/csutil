// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_CONTROLLER_H_
#define TEST_BASE_TEST_CONTROLLER_H_

#include <string>
#include <vector>

#include "base/status.h"
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

  /**
   * @brief 解析命令行参数
   *
   * 从命令行参数中提取 --gtest_filter= 参数，支持过滤测试用例
   *
   * @param argc 命令行参数数量
   * @param argv 命令行参数数组
   */
  void ParseCommandLine(int argc, char *argv[]);

 private:
  void ClearTestCases();
  void PrintTestInfoBeforeRun(const Test &test);
  void PrintTestInfoAfterRun(const Test &test, const struct timeval &begin_time, const struct timeval &end_time);
  void PrintTestCaseInfoBeforeRun(const std::pair<std::string, std::vector<Test *>> &test_case);
  void PrintTestCaseInfoAfterRun(const std::pair<std::string, std::vector<Test *>> &test_case,
                                 const struct timeval &begin_time, const struct timeval &end_time,
                                 int actual_tests_count);
  void PrintTestCasesInfoBeforeRun();
  void PrintTestCasesInfoAfterRun(const struct timeval &begin_time, const struct timeval &end_time);

  void RunDataCases(Test *test);

  void PrintDataTestInfoBeforeRun(const std::string &case_name, const std::string &case_desc);
  void PrintDataTestInfoAfterRun(const std::string &case_name, const struct timeval &begin_time,
                                 const struct timeval &end_time, bool is_data_driven_succ);
  void PrintDataTestCaseInfoBeforeRun(const Test *test, uint32_t data_cases_size);
  void PrintDataTestCaseInfoAfterRun(const Test *test, uint32_t data_cases_size, const struct timeval &begin_time,
                                     const struct timeval &end_time);

 private:
  /**
   * @brief 检查测试是否应该运行
   *
   * 根据过滤模式判断指定的测试用例是否应该执行
   *
   * @param test_case_name 测试用例组名称
   * @param test_name 测试名称
   * @return true 应该运行，false 跳过
   */
  bool ShouldRunTest(const std::string &test_case_name, const std::string &test_name) const;

  /**
   * @brief 解析过滤模式
   *
   * 将 "TestCase.TestName" 格式的过滤模式分离为测试用例名和测试名
   *
   * @param pattern 过滤模式字符串
   * @param test_case_pattern 输出：测试用例名模式
   * @param test_name_pattern 输出：测试名模式
   * @return base::kOk 成功
   * @return base::kInvalidParam 参数无效（test_case_pattern 或 test_name_pattern 为 NULL，或 pattern 为空）
   * @return base::kInvalidData 格式无效（点号在开头或结尾）
   */
  base::Code ParseFilterPattern(const std::string &pattern, std::string *test_case_pattern,
                                std::string *test_name_pattern) const;

  /**
   * @brief 匹配字符串模式
   *
   * 支持精确匹配和通配符 * 匹配（前缀匹配、后缀匹配、完全通配）
   *
   * @param str 要匹配的字符串
   * @param pattern 模式字符串
   * @return true 匹配成功，false 不匹配
   */
  bool MatchPattern(const std::string &str, const std::string &pattern) const;

 private:
  std::vector<std::pair<std::string, std::vector<Test *>>> test_cases_;

  int test_case_num_;
  int test_num_;
  int succ_test_num_;
  int fail_test_num_;

  std::vector<std::string> fail_tests_;

  std::string filter_pattern_;  // 过滤模式，如 "Search.BinarySearch_RangeSearch"

  // 实际执行的测试统计（用于过滤模式）
  int actual_test_case_num_;  // 实际执行的测试用例数量
  int actual_test_num_;       // 实际执行的测试数量
}; /*}}}*/

}  // namespace test

#endif
