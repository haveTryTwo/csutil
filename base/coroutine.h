// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COROUTINE_H_
#define BASE_COROUTINE_H_

#include <stdint.h>
#include <stdlib.h>

#include <ucontext.h>
#include <map>
#include <string>

#include "base/common.h"
#include "base/status.h"

namespace base {

class Dispatch;
typedef void (*CoroutineFunc)(Dispatch *dispatch, void *param);

struct Coroutine {
  CoroutineFunc func_;
  void *param_;
  ucontext_t context_;
  CoroutineStatus status_;
  std::string bak_stack_;

  Coroutine() { /*{{{*/
    func_ = NULL;
    param_ = NULL;
    status_ = kCoroutineNotExistStatus;
  } /*}}}*/

  ~Coroutine() { /*{{{*/
  }              /*}}}*/
};

class Dispatch {
 public:
  Dispatch();
  ~Dispatch();

 public:
  Code CreateCoroutine(CoroutineFunc func, void *param, uint32_t *coroutine_id);
  Code CoroutineResume(uint32_t coroutine_id);
  Code CoroutineYield();

 public:
  Code GetCurCoroutine(Coroutine **coroutine);
  Code GetCoroutine(uint32_t coroutine_id, Coroutine **coroutine);
  Code GetCoroutineStatus(uint32_t coroutine_id, CoroutineStatus *status);
  Code GetCoroutinesNum(uint32_t *coroutine_num);
  Code CheckCoroutineExist(uint32_t coroutine_id, bool *exist);
  Code DeleteCurCoroutine();

 private:
  Code GetNewCoroutineId(uint32_t *coroutine_id);

 private:
  char stack_[kMB];
  ucontext_t main_context_;
  uint32_t cur_coroutine_id_;
  std::map<uint32_t, Coroutine *> coroutines_;
};

}  // namespace base

#endif
