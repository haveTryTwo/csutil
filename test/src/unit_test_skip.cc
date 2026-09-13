// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test_base/include/test_base.h"

// NOTE:htt, 用于验证 test_base 的 TEST_SKIP 机制本身，详见
// docs/TEST_BASE_SKIP_TESTS_DESIGN.md。
//
// 若干用例体内有故意失败的 EXPECT：它们必须在 TEST_SKIP / TEST_SKIP_D 之后，
// 用来证明 return 截断生效。请勿删除或“修好”这些断言。

TEST(UnitTestSkip, Test_Normal_SkipInBody) { /*{{{*/
  TEST_SKIP();
  EXPECT_EQ(1, 2);
} /*}}}*/

TEST(UnitTestSkip, Test_Normal_SkipWithReason) { /*{{{*/
  TEST_SKIP_D("no gpu");
  EXPECT_EQ(1, 2);
} /*}}}*/

TEST(UnitTestSkip, Test_Normal_SkipDoesNotFail) { /*{{{*/
  TEST_SKIP();
} /*}}}*/

TEST(UnitTestSkip, Test_Normal_SkipThenUnreachableFail) { /*{{{*/
  TEST_SKIP();
  EXPECT_EQ(1, 2);
} /*}}}*/

TEST(UnitTestSkip, Test_Normal_NotSkipped) { /*{{{*/
  EXPECT_EQ(1, 1);
} /*}}}*/

class UnitTestSkipFixture : public test::Test {
 public:
  void Init() {
    test::Test::Init();
    fprintf(stderr, "UnitTestSkipFixture init\n");
    TEST_SKIP_D("skip in init");
  }

  void Destroy() {
    fprintf(stderr, "UnitTestSkipFixture destroy\n");
    test::Test::Destroy();
  }
};

TEST_F(UnitTestSkipFixture, Test_Normal_SkipInInit) { /*{{{*/
  // Init 已 skip 时不应执行到这里；若误跑会 FAIL
  EXPECT_EQ(1, 2);
} /*}}}*/

// NOTE:htt, 验证失败优先：EXPECT 失败后再 TEST_SKIP_D，最终必须是 FAIL。
// 默认不执行，需 --gtest_also_run_disabled_tests 放行，避免日常 ./test 红灯。
TEST(DISABLED_UnitTestSkipFailWins, Test_Exception_FailWinsOverSkip) { /*{{{*/
  EXPECT_EQ(1, 2);
  TEST_SKIP_D("should not change fail to skip");
} /*}}}*/

TEST_DATADRIVEN_D(UnitTestSkipData, Test_Normal_PartialSkip, "../data/json/test_skip_partial.txt",
                  const rapidjson::Value &data_case, "部分 skip 时父 Test 仍 PASS") { /*{{{*/
  EXPECT_EQ(true, data_case.HasMember("should_skip"));
  EXPECT_EQ(true, data_case["should_skip"].IsBool());
  if (data_case["should_skip"].GetBool()) {
    TEST_SKIP_D("not applicable");
  }
  EXPECT_EQ(1, 1);
} /*}}}*/

TEST_DATADRIVEN_D(UnitTestSkipDataAll, Test_Normal_AllSkip, "../data/json/test_skip_all.txt",
                  const rapidjson::Value &data_case, "全部 skip 时父 Test 为 SKIPPED") { /*{{{*/
  TEST_SKIP_D("all skip");
} /*}}}*/
