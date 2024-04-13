// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TEST_BASE_TEST_BASE_H_
#define TEST_BASE_TEST_BASE_H_

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
  void InitTest(const std::string &test_case_name, const std::string &test_name);

 public:
  void Begin();
  void End();

  virtual void Init();
  virtual void Destroy();

  virtual void ExecBody();

 public:
  const std::string &GetTestCaseName() const;
  void SetTestCaseName(const std::string &test_case_name);

  const std::string &GetTestName() const;
  void SetTestName(const std::string &test_name);

  bool GetIsSucc() const;
  void SetIsSucc(bool is_succ);

  const std::string &GetDesc() const;
  void SetDesc(const std::string &desc);

 private:
  std::string test_case_name_;
  std::string test_name_;
  std::string desc_;  // NOTE:htt, destination of current test

 private:
  bool is_succ_;
};

}  // namespace test

#define TEST_CLASS_NAME_(test_case_name, test_name) test_case_name##_##test_name

#define TEST_CLASS_TO_OBJECT_NAME_(test_case_name, test_name) \
  test_case_name##_##test_name##_test_obj

test::Test *MakeRegister(const std::string &test_case_name, const std::string &test_name,
                         test::Test *test_obj, const std::string &desc);

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

template <typename T>
int CheckEqual(const T &expect, const T &real) {
  if (expect.size() != real.size()) return -1;

  typename T::const_iterator expect_it;
  typename T::const_iterator real_it;
  for (expect_it = expect.begin(), real_it = real.begin();
       expect_it != expect.end(), real_it != real.end(); ++expect_it, ++real_it) {
    if (*expect_it != *real_it) return -1;
  }

  return 0;
}

#endif
