// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/event_loop.h"

namespace base {

EventLoop::EventLoop() : actions_(), evt_(NULL) { /*{{{*/ } /*}}}*/

EventLoop::~EventLoop() { /*{{{*/
  actions_.clear();

  if (evt_ != NULL) {
    delete evt_;
    evt_ = NULL;
  }
} /*}}}*/

Code EventLoop::Init(EventType evt_type) { /*{{{*/
  Code ret = kOk;
  switch (evt_type) {
    case kSelect:
      break;
    case kPoll:
      evt_ = new EventPoll();
      ret = evt_->Create(kDefaultSizeOfFds);
      break;
    case kEPoll:
#if defined(__linux__)
      evt_ = new EventEpoll();
#else
      evt_ = new EventPoll();
#endif
      ret = evt_->Create(kDefaultSizeOfFds);
      break;
    default:
      evt_ = new EventPoll();
      ret = evt_->Create(kDefaultSizeOfFds);
      break;
  }

  return ret;
} /*}}}*/

Code EventLoop::Add(int fd, int evt, EventFunc func, void *param) { /*{{{*/
  Code ret = evt_->Add(fd, evt);
  if (ret != kOk) return ret;

  EventItem item = {fd, evt, func, param};
  actions_.insert(std::pair<int, EventItem>(fd, item));

  return ret;
} /*}}}*/

Code EventLoop::Mod(int fd, int evt, EventFunc func, void *param) { /*{{{*/
  // 先检查 fd 是否存在于 actions_ 中
  std::map<int, EventItem>::iterator it = actions_.find(fd);
  if (it == actions_.end()) {
    return kNotFound;
  }

  Code ret = evt_->Mod(fd, evt);
  if (ret != kOk) return ret;

  // 更新已存在的事件项
  EventItem &item = it->second;
  item.fd = fd;
  item.evt = evt;
  item.func = func;
  item.param = param;

  return ret;
} /*}}}*/

Code EventLoop::Del(int fd) { /*{{{*/
  // 先从底层事件系统删除
  Code ret = evt_->Del(fd);
  if (ret != kOk) return ret;

  // 成功后再从 actions_ 中删除
  actions_.erase(fd);

  return ret;
} /*}}}*/

Code EventLoop::Run() { /*{{{*/
  Code ret = kOk;

  while (true) {
    ret = evt_->Wait(kDefaultWaitTimeMs);
    if (ret != kOk) continue;

    // 处理所有就绪的事件
    while (true) {
      int fd = -1;
      int evt = 0;
      ret = evt_->GetEvents(&fd, &evt);
      if (ret != kOk) break;

      typedef std::map<int, EventItem>::iterator Iter;
      Iter it = actions_.find(fd);
      if (it == actions_.end()) {
        // fd 不在 actions_ 中，说明数据不一致
        // 记录错误但继续处理其他事件
        continue;
      }

      // 执行事件回调函数
      Code callback_ret = (it->second).func(fd, evt, (it->second).param);
      // 可以根据回调返回值决定是否继续处理其他事件
      // 如果回调返回错误，可以选择记录日志但继续处理
      (void)callback_ret;  // 当前忽略回调返回值
    }
  }

  return ret;
} /*}}}*/

}  // namespace base

#ifdef _EVENT_LOOP_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;
  EventLoop main_loop;
  main_loop.Init(kPoll);

  return 0;
} /*}}}*/
#endif
