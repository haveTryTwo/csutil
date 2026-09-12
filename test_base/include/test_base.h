// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_BASE_H_
#define TEST_BASE_TEST_BASE_H_

#include "rapidjson/document.h"

#include "base/time.h"
#include "strategy/singleton.h"

#include "test_base/include/test_controller.h"

namespace test {

class TestController;

class Test {
 public:
  Test();
  virtual ~Test();

 public:
  void InitTest(const std::string &test_case_name, const std::string &test_name,
                bool is_data_driven, const std::string &data_driven_path);

 public:
  void Begin();
  void End();

  virtual void Init();
  virtual void Destroy();

  virtual void ExecBody();
  virtual void ExecBody(const rapidjson::Value &value);

 public:
  const std::string &GetTestCaseName() const;
  void SetTestCaseName(const std::string &test_case_name);

  const std::string &GetTestName() const;
  void SetTestName(const std::string &test_name);

  bool GetIsSucc() const;
  void SetIsSucc(bool is_succ);

  const std::string &GetDesc() const;
  void SetDesc(const std::string &desc);

  const std::string &GetDataDrivenPath() const;
  void SetDataDrivenPath(const std::string &data_driven_path);

  bool GetIsDataDriven() const;
  void SetIsDataDriven(bool is_data_driven);

  bool GetIsDataDrivenSucc() const;
  void SetIsDataDrivenSucc(bool is_data_driven_succ);

  /**
   * @brief 是否被 DISABLED_ 前缀标记为禁用
   *
   * 判定规则对齐 gtest：test_case_name 或 test_name 任一以字面量 "DISABLED_" 开头，
   * 即认为该测试被禁用；默认不会被 TestController::Run() 执行，
   * 除非命令行指定 --gtest_also_run_disabled_tests（或设置环境变量
   * GTEST_ALSO_RUN_DISABLED_TESTS 为非 0 值）。
   *
   * @return true 已禁用；false 未禁用
   */
  bool IsDisabled() const;

  /**
   * @brief 将当前测试标记为跳过或取消跳过
   *
   * 只置标志，不负责退出当前函数。用户应使用 TEST_SKIP / TEST_SKIP_D，
   * 不要直接调用本方法。失败优先：若此前 EXPECT 已 SetIsSucc(false)，最终仍是 FAIL。
   *
   * @param is_skipped true 标记为跳过；false 清除跳过标志并清空理由
   */
  void SetIsSkipped(bool is_skipped);

  /**
   * @brief 当前是否已被标记为跳过
   * @return true 已跳过；false 未跳过
   */
  bool IsSkipped() const;

  /**
   * @brief 设置跳过原因，允许空字符串
   * @param reason 跳过原因
   */
  void SetSkipReason(const std::string &reason);

  /**
   * @brief 获取跳过原因
   * @return 跳过原因，未设置时为空字符串
   */
  const std::string &GetSkipReason() const;

  /**
   * @brief 清除跳过标志与理由（数据驱动的下一条用例开始前调用）
   */
  void ClearIsSkipped();

 private:
  std::string test_case_name_;
  std::string test_name_;
  std::string desc_;              // NOTE:htt, destination of current test
  std::string data_driven_path_;  // NOTE:htt, path of data driven test cases
  std::string skip_reason_;       // NOTE:htt, reason of TEST_SKIP / TEST_SKIP_D

 private:
  bool is_succ_;
  bool is_data_driven_;
  bool is_data_driven_succ_;
  bool is_skipped_;
};

}  // namespace test

#define TEST_CLASS_NAME_(test_case_name, test_name) test_case_name##_##test_name

#define TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name) \
  test_case_name##_##test_name##_test_obj

test::Test *MakeRegister(const std::string &test_case_name, const std::string &test_name,
                         test::Test *test_obj, const std::string &desc);

test::Test *MakeRegister(const std::string &test_case_name, const std::string &test_name,
                         test::Test *test_obj, bool is_data_driven,
                         const std::string &data_driven_path, const std::string &desc);

// NOTE:htt, 构建代码驱动测试框架
#define TEST_INTERNAL_(test_case_name, test_name, father_class, desc)                        \
  class TEST_CLASS_NAME_(test_case_name, test_name) : public father_class {                  \
   public:                                                                                   \
    TEST_CLASS_NAME_(test_case_name, test_name)() : father_class() {}                        \
                                                                                             \
   public:                                                                                   \
    virtual void ExecBody();                                                                 \
  };                                                                                         \
  test::Test *TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name) = MakeRegister(          \
      #test_case_name, #test_name, new TEST_CLASS_NAME_(test_case_name, test_name)(), desc); \
  void TEST_CLASS_NAME_(test_case_name, test_name)::ExecBody()

#define TEST(test_case_name, test_name) TEST_INTERNAL_(test_case_name, test_name, test::Test, "")

#define TEST_D(test_case_name, test_name, desc) \
  TEST_INTERNAL_(test_case_name, test_name, test::Test, desc)

#define TEST_F(test_case_name, test_name) \
  TEST_INTERNAL_(test_case_name, test_name, test_case_name, "")

#define TEST_F_D(test_case_name, test_name, desc) \
  TEST_INTERNAL_(test_case_name, test_name, test_case_name, desc)

// NOTE:htt, 构建数据驱动测试框架
#define TEST_DATADRIVEN_INTERNAL_(test_case_name, test_name, father_class, data_driven_path,       \
                                  data_case, desc)                                                 \
  class TEST_CLASS_NAME_(test_case_name, test_name) : public father_class {                        \
   public:                                                                                         \
    TEST_CLASS_NAME_(test_case_name, test_name)() : father_class() {}                              \
                                                                                                   \
   public:                                                                                         \
    virtual void ExecBody(const rapidjson::Value &value);                                          \
  };                                                                                               \
  test::Test *TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name) =                              \
      MakeRegister(#test_case_name, #test_name, new TEST_CLASS_NAME_(test_case_name, test_name)(), \
                   true, data_driven_path, desc);                                                  \
  void TEST_CLASS_NAME_(test_case_name, test_name)::ExecBody(data_case)

#define TEST_DATADRIVEN(test_case_name, test_name, data_driven_path, data_case) \
  TEST_DATADRIVEN_INTERNAL_(test_case_name, test_name, test::Test, data_driven_path, data_case, "")

#define TEST_DATADRIVEN_D(test_case_name, test_name, data_driven_path, data_case, desc)         \
  TEST_DATADRIVEN_INTERNAL_(test_case_name, test_name, test::Test, data_driven_path, data_case, \
                            desc)

/**
 * @brief 跳过当前测试（无理由）
 *
 * 必须写在 Init / ExecBody / ExecBody(const rapidjson::Value &) 内，不要写进普通 helper。
 * 不支持在 Destroy 中改变本条结果。
 */
#define TEST_SKIP()     \
  do {                  \
    SetIsSkipped(true); \
    return;             \
  } while (0)

/**
 * @brief 跳过当前测试并记录原因
 *
 * 必须写在 Init / ExecBody / ExecBody(const rapidjson::Value &) 内，不要写进普通 helper。
 * 不支持在 Destroy 中改变本条结果。
 *
 * @param reason 跳过原因
 */
#define TEST_SKIP_D(reason) \
  do {                      \
    SetSkipReason(reason);  \
    SetIsSkipped(true);     \
    return;                 \
  } while (0)

// NOTE:htt, EXPACT 判断
#define EXPECT_EQ(expect_val, real_val)                       \
  if ((expect_val) != (real_val)) {                           \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_NEQ(expect_val, real_val)                      \
  if ((expect_val) == (real_val)) {                           \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_NE(expect_val, real_val) EXPECT_NEQ(expect_val, real_val)

#define EXPECT_GT(expect_val, real_val)                       \
  if ((expect_val) <= (real_val)) {                           \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_GE(expect_val, real_val)                       \
  if ((expect_val) < (real_val)) {                            \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_LT(expect_val, real_val)                       \
  if ((expect_val) >= (real_val)) {                           \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_LE(expect_val, real_val)                       \
  if ((expect_val) > (real_val)) {                            \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_NEAR(expect_val, real_val, near_val)           \
  if (abs((expect_val) - (real_val)) > abs(near_val)) {       \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_TRUE(expect_val)                               \
  if (!(expect_val)) {                                        \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_FALSE(expect_val)                              \
  if (expect_val) {                                           \
    SetIsSucc(false);                                         \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_TEST_EQ(expect_val, real_val, test)            \
  if ((expect_val) != (real_val)) {                           \
    test->SetIsSucc(false);                                   \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

#define EXPECT_TEST_NEQ(expect_val, real_val, test)           \
  if ((expect_val) == (real_val)) {                           \
    test->SetIsSucc(false);                                   \
    fprintf(stderr, "(%s %d) Failed!\n", __FILE__, __LINE__); \
  }

template <typename T>
int CheckEqual(const T &expect, const T &real) {
  if (expect.size() != real.size()) return -1;

  typename T::const_iterator expect_it;
  typename T::const_iterator real_it;
  for (expect_it = expect.begin(), real_it = real.begin();
       (expect_it != expect.end()) && (real_it != real.end()); ++expect_it, ++real_it) {
    if (*expect_it != *real_it) return -1;
  }

  return 0;
}

#endif
