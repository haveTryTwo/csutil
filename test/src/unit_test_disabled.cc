// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test_base/include/test_base.h"

// NOTE:htt, 用于验证 test_base 的 DISABLED_ 机制本身，详见
// docs/TEST_BASE_DISABLED_TESTS_DESIGN.md。
//
// 下面两个 DISABLED_ 用例的断言是**刻意失败**的：只有在它们被错误执行时才会
// FAIL，用来证明"默认应跳过"这一预期是否被破坏。请勿删除或"修好"这两个断言。

TEST(UnitTestDisabled, DISABLED_Test_Normal_ShouldSkipByDefault) { /*{{{*/
  // 默认（不带 --gtest_also_run_disabled_tests）不应执行到这里
  EXPECT_EQ(1, 2);
} /*}}}*/

TEST(DISABLED_UnitTestDisabledSuite, Test_Normal_PlainName) { /*{{{*/
  // suite 名带 DISABLED_ 前缀，同样默认不应执行到这里
  EXPECT_EQ(1, 2);
} /*}}}*/

TEST(UnitTestDisabled, Test_Normal_NotDisabled) { /*{{{*/
  // 未被禁用的普通用例：必须始终被执行且通过，验证禁用用例不影响同 suite 内其他用例
  EXPECT_EQ(1, 1);
} /*}}}*/
