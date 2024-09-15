// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/coroutine.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

void CoroutineFuncOne(base::Dispatch *dispatch, void *param) { /*{{{*/
  int *max_num = (int *)(param);
  for (int i = 0; i < *max_num; ++i) {
    fprintf(stderr, "Nice One Coroutine! i:%d, max_num:%d\n", i, *max_num);
    dispatch->CoroutineYield();
  }
} /*}}}*/

void CoroutineFuncTwo(base::Dispatch *dispatch, void *param) { /*{{{*/
  int *max_num = (int *)(param);
  for (int i = 0; i < *max_num; ++i) {
    fprintf(stderr, "Nice Two Coroutine! i:%d, max_num:%d\n", i, *max_num);
    dispatch->CoroutineYield();
  }
} /*}}}*/

TEST(Disptach, Test_Normal) { /*{{{*/
  using namespace base;
  Dispatch *dispatch = new Dispatch();

  int max_one_num = 3;
  uint32_t one_cor_id = 0;
  uint32_t two_cor_id = 0;
  Code ret = dispatch->CreateCoroutine(CoroutineFuncOne, &max_one_num, &one_cor_id);
  EXPECT_EQ(kOk, ret);

  int max_two_num = 5;
  ret = dispatch->CreateCoroutine(CoroutineFuncTwo, &max_two_num, &two_cor_id);
  EXPECT_EQ(kOk, ret);

  bool exist = false;
  uint32_t coroutines_num = 0;
  CoroutineStatus status = kCoroutineNotExistStatus;
  while (true) {
    ret = dispatch->GetCoroutinesNum(&coroutines_num);
    EXPECT_EQ(kOk, ret);
    if (coroutines_num == 0) {
      break;
    }

    ret = dispatch->CheckCoroutineExist(one_cor_id, &exist);
    EXPECT_EQ(kOk, ret);
    if (exist) {
      ret = dispatch->CoroutineResume(one_cor_id);  // NOTE: execute Coroutine and Yield from it to main context
      EXPECT_EQ(kOk, ret);

      ret = dispatch->GetCoroutineStatus(one_cor_id, &status);
      EXPECT_EQ(kOk, ret);
      if (status == kCoroutineNotExistStatus) {
        fprintf(stderr, "Coroutine:%u finish!\n", (unsigned int)one_cor_id);
      }
    }

    ret = dispatch->CheckCoroutineExist(two_cor_id, &exist);
    EXPECT_EQ(kOk, ret);
    if (exist) {
      ret = dispatch->CoroutineResume(two_cor_id);
      EXPECT_EQ(kOk, ret);

      ret = dispatch->GetCoroutineStatus(two_cor_id, &status);
      EXPECT_EQ(kOk, ret);
      if (status == kCoroutineNotExistStatus) {
        fprintf(stderr, "Coroutine:%u finish!\n", (unsigned int)two_cor_id);
      }
    }
  }

  delete dispatch;
  dispatch = NULL;
} /*}}}*/

TEST(Disptach, Test_Exception_Resume_After_Finish) { /*{{{*/
  using namespace base;
  Dispatch *dispatch = new Dispatch();

  int max_one_num = 2;
  uint32_t one_cor_id = 0;
  Code ret = dispatch->CreateCoroutine(CoroutineFuncOne, &max_one_num, &one_cor_id);
  EXPECT_EQ(kOk, ret);

  bool exist = false;
  uint32_t coroutines_num = 0;
  CoroutineStatus status = kCoroutineNotExistStatus;
  while (true) {
    ret = dispatch->GetCoroutinesNum(&coroutines_num);
    EXPECT_EQ(kOk, ret);
    if (coroutines_num == 0) {
      break;
    }

    ret = dispatch->CheckCoroutineExist(one_cor_id, &exist);
    EXPECT_EQ(kOk, ret);
    if (exist) {
      ret = dispatch->CoroutineResume(one_cor_id);
      EXPECT_EQ(kOk, ret);

      ret = dispatch->GetCoroutineStatus(one_cor_id, &status);
      EXPECT_EQ(kOk, ret);
      if (status == kCoroutineNotExistStatus) {
        fprintf(stderr, "Coroutine:%u finish!\n", (unsigned int)one_cor_id);
      }
    }
  }

  ret = dispatch->CoroutineResume(one_cor_id);
  EXPECT_EQ(kInvalidCoroutineId, ret);

  delete dispatch;
  dispatch = NULL;
} /*}}}*/

void CoroutineFuncThree(base::Dispatch *dispatch, void *param) { /*{{{*/
  int *max_num = (int *)(param);
  for (int i = 0; i < *max_num; ++i) {
    fprintf(stderr, "Nice Two Coroutine! i:%d, max_num:%d\n", i, *max_num);
    dispatch->CoroutineYield();
    dispatch->CoroutineYield();
  }
} /*}}}*/

TEST(Disptach, Test_Exception_Yield_Twice) { /*{{{*/
  using namespace base;
  Dispatch *dispatch = new Dispatch();

  int max_one_num = 3;
  uint32_t one_cor_id = 0;
  Code ret = dispatch->CreateCoroutine(CoroutineFuncThree, &max_one_num, &one_cor_id);
  EXPECT_EQ(kOk, ret);

  bool exist = false;
  uint32_t coroutines_num = 0;
  CoroutineStatus status = kCoroutineNotExistStatus;
  while (true) {
    ret = dispatch->GetCoroutinesNum(&coroutines_num);
    EXPECT_EQ(kOk, ret);
    if (coroutines_num == 0) {
      break;
    }

    ret = dispatch->CheckCoroutineExist(one_cor_id, &exist);
    EXPECT_EQ(kOk, ret);
    if (exist) {
      ret = dispatch->CoroutineResume(one_cor_id);
      EXPECT_EQ(kOk, ret);

      ret = dispatch->GetCoroutineStatus(one_cor_id, &status);
      EXPECT_EQ(kOk, ret);
      if (status == kCoroutineNotExistStatus) {
        fprintf(stderr, "Coroutine:%u finish!\n", (unsigned int)one_cor_id);
      }
    }
  }

  delete dispatch;
  dispatch = NULL;
} /*}}}*/
