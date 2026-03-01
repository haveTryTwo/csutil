// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "base/event_poll.h"

namespace base {

EventPoll::EventPoll() { /*{{{*/
  pfds_ = NULL;
  all_num_of_pfds_ = 0;
  used_num_of_pfds_ = 0;
  del_num_of_pfds_ = 0;

  evt_num_ = 0;
  cur_evt_num_ = 0;
} /*}}}*/

EventPoll::~EventPoll() { /*{{{*/
  if (pfds_ != NULL) {
    delete[] pfds_;
    pfds_ = NULL;
  }
} /*}}}*/

Code EventPoll::Create(int nfds) { /*{{{*/
  if (nfds <= 0) return kInvalidParam;

  // 避免重复创建导致内存泄漏
  if (pfds_ != NULL) {
    delete[] pfds_;
    pfds_ = NULL;
  }

  all_num_of_pfds_ = nfds;
  pfds_ = new struct pollfd[nfds];
  assert(pfds_ != NULL);
  memset(pfds_, -1, sizeof(struct pollfd) * nfds);

  return kOk;
} /*}}}*/

Code EventPoll::Wait(int time_out_ms) { /*{{{*/
  if (del_num_of_pfds_ > 0) Balance();

  while (true) {
    evt_num_ = poll(pfds_, used_num_of_pfds_, time_out_ms);
    if (evt_num_ == -1 && errno == EINTR)
      continue;
    else
      break;
  }
  if (evt_num_ == -1)
    return kPollError;
  else if (evt_num_ == 0)
    return kTimeOut;

  cur_evt_num_ = 0;
  cur_idx_of_pfds_ = 0;

  return kOk;
} /*}}}*/

Code EventPoll::GetEvents(int *fd, int *evt) { /*{{{*/
  if (fd == NULL || evt == NULL) return kInvalidParam;

  if (evt_num_ <= 0) return kNotFound;

  while (cur_idx_of_pfds_ < used_num_of_pfds_ && cur_evt_num_ < evt_num_) {
    if (pfds_[cur_idx_of_pfds_].fd != -1 && pfds_[cur_idx_of_pfds_].revents != 0) {
      *fd = pfds_[cur_idx_of_pfds_].fd;
      *evt = pfds_[cur_idx_of_pfds_].revents;
      ++cur_idx_of_pfds_;
      ++cur_evt_num_;
      return kOk;
    }
    ++cur_idx_of_pfds_;
  }

  return kNotFound;
} /*}}}*/

Code EventPoll::Add(int fd, int evt) { /*{{{*/
  // 检查 fd 是否已经存在
  for (int i = 0; i < used_num_of_pfds_; ++i) {
    if (pfds_[i].fd == fd) {
      return kAlreadyExist;  // fd 已存在，返回错误
    }
  }

  // 如果有删除的空间，先进行 rebalance
  if (del_num_of_pfds_ > 0 && used_num_of_pfds_ >= all_num_of_pfds_) {
    Balance();
  }

  if (used_num_of_pfds_ >= all_num_of_pfds_) return kFull;

  pfds_[used_num_of_pfds_].fd = fd;
  pfds_[used_num_of_pfds_].events = evt;
  pfds_[used_num_of_pfds_].revents = 0;

  used_num_of_pfds_++;

  return kOk;
} /*}}}*/

/**
 * @brief 修改文件描述符监听的事件类型
 * @param fd 文件描述符
 * @param evt 新的事件类型
 * @return kOk 成功；kNotFound fd 不存在
 */
Code EventPoll::Mod(int fd, int evt) { /*{{{*/
  if (fd < 0) return kInvalidParam;

  for (int i = 0; i < used_num_of_pfds_; ++i) {
    if (pfds_[i].fd == fd) {
      pfds_[i].events = evt;
      return kOk;
    }
  }

  return kNotFound;
} /*}}}*/

/**
 * @brief 从事件循环中删除文件描述符
 * @param fd 文件描述符
 * @return kOk 成功；kNotFound fd 不存在
 *
 * 注意：删除操作只是标记 fd 为 -1，真正的清理在 Balance() 中完成
 */
Code EventPoll::Del(int fd) { /*{{{*/
  if (fd < 0) return kInvalidParam;

  for (int i = 0; i < used_num_of_pfds_; ++i) {
    if (pfds_[i].fd == fd) {
      pfds_[i].fd = -1;
      ++del_num_of_pfds_;
      return kOk;
    }
  }

  return kNotFound;
} /*}}}*/

void EventPoll::Print() { /*{{{*/
  fprintf(stderr, "\033[1;31m%10s | %10s | %10s\033[0m\n", "fd", "events", "revents");
  for (int i = 0; i < used_num_of_pfds_; ++i) {
    fprintf(stderr, "%10d | %10d | %10d\n", pfds_[i].fd, pfds_[i].events, pfds_[i].revents);
  }
} /*}}}*/

/**
 * @brief 整理 pollfd 数组，将被删除的项移到末尾
 * @return kOk 成功
 *
 * 注意：将所有 fd == -1 的项移到数组末尾，减少 used_num_of_pfds_
 */
Code EventPoll::Balance() { /*{{{*/
  int start_pos = 0;
  int end_pos = used_num_of_pfds_ - 1;

  while (start_pos < end_pos) {
    // 找到第一个被删除的位置（fd == -1）
    while (start_pos < end_pos && pfds_[start_pos].fd != -1) {
      ++start_pos;
    }

    // 从后往前找到第一个有效的位置（fd != -1）
    while (start_pos < end_pos && pfds_[end_pos].fd == -1) {
      --end_pos;
    }

    // 只有当 start_pos < end_pos 时才需要交换
    if (start_pos < end_pos) {
      Swap(pfds_ + start_pos, pfds_ + end_pos);
    }
  }

  used_num_of_pfds_ -= del_num_of_pfds_;
  del_num_of_pfds_ = 0;

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _EVENT_POLL_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  Event *ev = new EventPoll();
  ev->Create(100);
  ev->Print();

  // int time_out_ms = 1000;

  for (int i = 0; i < 7; ++i) {
    ev->Add(i, POLLIN | POLLOUT);
  }
  ev->Print();

  ev->Del(2);
  ev->Del(4);
  ev->Del(22);
  ev->Print();

  return 0;
} /*}}}*/
#endif
