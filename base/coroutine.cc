// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/coroutine.h"

#include <assert.h>
#include <string.h>

namespace base {

static void MainFunc(void *param) { /*{{{*/
  Dispatch *dispatch = (Dispatch *)param;
  Coroutine *cur_coroutine = NULL;
  Code ret = dispatch->GetCurCoroutine(&cur_coroutine);
  if (ret != kOk) return;

  cur_coroutine->func_(dispatch, cur_coroutine->param_);
  dispatch->DeleteCurCoroutine();
} /*}}}*/

Dispatch::Dispatch() { /*{{{*/
  memset(stack_, '\0', sizeof(stack_));
  cur_coroutine_id_ = kCoroutineDefaultId;
} /*}}}*/

Dispatch::~Dispatch() { /*{{{*/
  std::map<uint32_t, Coroutine *>::iterator it = coroutines_.begin();
  for (; it != coroutines_.end(); ++it) {
    delete it->second;
    it->second = NULL;
  }
  coroutines_.clear();
} /*}}}*/

Code Dispatch::CreateCoroutine(CoroutineFunc func, void *param, uint32_t *coroutine_id) { /*{{{*/
  if (coroutine_id == NULL) return kInvalidParam;

  uint32_t new_coroutine_id = 0;
  Code ret = GetNewCoroutineId(&new_coroutine_id);
  if (ret != kOk) return ret;

  Coroutine *coroutine = new Coroutine();
  coroutine->func_ = func;
  coroutine->param_ = param;
  coroutine->status_ = kCoroutinePreparingStatus;

  coroutines_.insert(std::pair<uint32_t, Coroutine *>(new_coroutine_id, coroutine));
  *coroutine_id = new_coroutine_id;

  return kOk;
} /*}}}*/

Code Dispatch::CoroutineResume(uint32_t coroutine_id) { /*{{{*/
  Coroutine *cur_coroutine = NULL;
  Code ret = GetCoroutine(coroutine_id, &cur_coroutine);
  if (ret != kOk) return ret;

  int r = 0;
  switch (cur_coroutine->status_) {
    case kCoroutinePreparingStatus:
      r = getcontext(&(cur_coroutine->context_));
      if (r == -1) return kFailedGetContext;
      cur_coroutine->status_ = kCoroutineRunningStatus;
      cur_coroutine_id_ = coroutine_id;
      cur_coroutine->context_.uc_stack.ss_sp = stack_;
      cur_coroutine->context_.uc_stack.ss_size = sizeof(stack_);
      cur_coroutine->context_.uc_link = NULL;
      makecontext(&(cur_coroutine->context_), (void (*)())MainFunc, 1, (void *)this);
      swapcontext(&main_context_, &(cur_coroutine->context_));

      break;
    case kCoroutineSuspendingStatus:
      memcpy(stack_ + sizeof(stack_) - cur_coroutine->bak_stack_.size(), cur_coroutine->bak_stack_.data(),
             cur_coroutine->bak_stack_.size());  // NOTE: resume stack
      cur_coroutine->status_ = kCoroutineRunningStatus;
      cur_coroutine_id_ = coroutine_id;
      swapcontext(&main_context_, &(cur_coroutine->context_));
      break;
    default:
      return kInvalidStatus;
  }

  return kOk;
} /*}}}*/

Code Dispatch::CoroutineYield() { /*{{{*/
  Coroutine *cur_coroutine = NULL;
  Code ret = GetCurCoroutine(&cur_coroutine);
  if (ret != kOk) return ret;

  // fprintf(stderr, "&cur_coroutine:%p, stack_:%p, top:%p\n", (char*)&cur_coroutine, stack_,
  // stack_+sizeof(stack_)); Note: the stack of current coroutine must be in [stack_,
  // stack_+sizeof(stack_)]
  assert(((char *)&cur_coroutine < stack_ + sizeof(stack_)) && ((char *)&cur_coroutine >= stack_));

  // Note: keeping current coroutine's stack
  cur_coroutine->bak_stack_.assign((char *)(&cur_coroutine), stack_ + sizeof(stack_) - (char *)(&cur_coroutine));
  cur_coroutine->status_ = kCoroutineSuspendingStatus;
  cur_coroutine_id_ = kCoroutineDefaultId;
  int r = swapcontext(&(cur_coroutine->context_), &main_context_);
  if (r == -1) return kFailedSwapContext;
  return kOk;
} /*}}}*/

Code Dispatch::GetNewCoroutineId(uint32_t *coroutine_id) { /*{{{*/
  if (coroutine_id == NULL) return kInvalidParam;

  if (coroutines_.size() == 0) {
    *coroutine_id = kCoroutineStartId;
    return kOk;
  }

  if (coroutines_.size() > kMaxCoroutines) {
    return kCoroutinesTooMach;
  }

  std::map<uint32_t, Coroutine *>::reverse_iterator rit = coroutines_.rbegin();
  uint32_t biggest_coroutine_id = rit->first;
  for (uint32_t tmp_id = biggest_coroutine_id + 1;; ++tmp_id) {
    if (tmp_id == kCoroutineDefaultId) {
      continue;
    }

    if (coroutines_.find(tmp_id) == coroutines_.end()) {
      *coroutine_id = tmp_id;
      return kOk;
    }
  }

  return kOtherFailed;
} /*}}}*/

Code Dispatch::GetCurCoroutine(Coroutine **coroutine) { /*{{{*/
  if (coroutine == NULL) return kInvalidParam;

  if (cur_coroutine_id_ == kCoroutineDefaultId) return kInvalidCoroutineId;
  return GetCoroutine(cur_coroutine_id_, coroutine);
} /*}}}*/

Code Dispatch::GetCoroutine(uint32_t coroutine_id, Coroutine **coroutine) { /*{{{*/
  if (coroutine == NULL) return kInvalidParam;
  std::map<uint32_t, Coroutine *>::iterator cur_it = coroutines_.find(coroutine_id);
  if (cur_it == coroutines_.end()) {
    return kInvalidCoroutineId;
  }

  *coroutine = cur_it->second;
  return kOk;
} /*}}}*/

Code Dispatch::GetCoroutineStatus(uint32_t coroutine_id, CoroutineStatus *status) { /*{{{*/
  if (status == NULL) return kInvalidParam;
  std::map<uint32_t, Coroutine *>::iterator cur_it = coroutines_.find(coroutine_id);
  if (cur_it != coroutines_.end()) {
    *status = cur_it->second->status_;
  } else {
    *status = kCoroutineNotExistStatus;
  }

  return kOk;
} /*}}}*/

Code Dispatch::GetCoroutinesNum(uint32_t *coroutine_num) { /*{{{*/
  if (coroutine_num == NULL) return kInvalidParam;
  *coroutine_num = coroutines_.size();
  return kOk;
} /*}}}*/

Code Dispatch::CheckCoroutineExist(uint32_t coroutine_id, bool *exist) { /*{{{*/
  if (exist == NULL) return kInvalidParam;
  *exist = false;
  std::map<uint32_t, Coroutine *>::iterator cur_it = coroutines_.find(coroutine_id);
  if (cur_it != coroutines_.end()) {
    *exist = true;
  }

  return kOk;
} /*}}}*/

Code Dispatch::DeleteCurCoroutine() { /*{{{*/
  if (cur_coroutine_id_ == kCoroutineDefaultId) return kInvalidCoroutineId;

  std::map<uint32_t, Coroutine *>::iterator cur_it = coroutines_.find(cur_coroutine_id_);
  if (cur_it == coroutines_.end()) {
    return kInvalidCoroutineId;
  }
  Coroutine *cur_coroutine = cur_it->second;

  delete cur_coroutine;
  coroutines_.erase(cur_it);

  cur_coroutine_id_ = kCoroutineDefaultId;

  ucontext_t context;
  swapcontext(&context, &main_context_);

  return kOk;
} /*}}}*/

}  // namespace base
