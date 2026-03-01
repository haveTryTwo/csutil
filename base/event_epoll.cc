// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/event_epoll.h"

#include <errno.h>
#include <unistd.h>

namespace base {

EventEpoll::EventEpoll() : epfd_(-1), evts_(NULL), max_evts_num_(0), cur_evts_pos_(0), evts_num_(0) {}

EventEpoll::~EventEpoll() { /*{{{*/
  if (epfd_ != -1) {
    close(epfd_);
    epfd_ = -1;
  }

  if (evts_ != NULL) {
    delete[] evts_;
    evts_ = NULL;
  }
} /*}}}*/

/**
 * @brief 创建事件循环实例
 * @param nfds 支持的最大文件描述符数量
 * @return kOk 成功；kInvalidParam 参数无效；kEpollCreateFailed/kEpollFailed 创建失败
 *
 * 注意：多次调用会导致资源泄漏，应该只调用一次
 */
Code EventEpoll::Create(int nfds) { /*{{{*/
  if (nfds <= 0) return kInvalidParam;

  epfd_ = epoll_create(nfds);
  if (epfd_ == -1) return kEpollCreateFailed;

  evts_ = new struct epoll_event[nfds];
  if (evts_ == NULL) {
    close(epfd_);
    epfd_ = -1;
    return kEpollFailed;
  }
  max_evts_num_ = nfds;

  return kOk;
} /*}}}*/

/**
 * @brief 等待事件发生
 * @param time_out_ms 超时时间（毫秒），-1 表示永久等待
 * @return kOk 有事件就绪；kTimeOut 超时；kEpollFailed 失败
 *
 * 注意：会自动重试 EINTR 错误（信号中断）
 */
Code EventEpoll::Wait(int time_out_ms) { /*{{{*/
  while (true) {
    evts_num_ = epoll_wait(epfd_, evts_, max_evts_num_, time_out_ms);
    if (evts_num_ == -1 && errno == EINTR)
      continue;
    else
      break;
  }
  if (evts_num_ == -1) return kEpollFailed;
  if (evts_num_ == 0) return kTimeOut;

  cur_evts_pos_ = 0;

  return kOk;
} /*}}}*/

/**
 * @brief 获取下一个就绪的事件
 * @param fd 输出参数，返回就绪的文件描述符
 * @param evt 输出参数，返回事件类型
 * @return kOk 成功获取事件；kNotFound 没有更多事件
 *
 * 注意：需要在 Wait() 返回 kOk 后调用，可以多次调用直到返回 kNotFound
 */
Code EventEpoll::GetEvents(int *fd, int *evt) { /*{{{*/
  if (fd == NULL || evt == NULL) return kInvalidParam;

  if (cur_evts_pos_ < evts_num_) {
    *evt = evts_[cur_evts_pos_].events;
    *fd = evts_[cur_evts_pos_].data.fd;
    ++cur_evts_pos_;
    return kOk;
  }

  return kNotFound;
} /*}}}*/

/**
 * @brief 向事件循环添加文件描述符
 * @param fd 文件描述符
 * @param evt 监听的事件类型（EPOLLIN/EPOLLOUT 等）
 * @return kOk 成功；kEpollCtlFailed 添加失败
 *
 * 注意：如果 fd 已存在，epoll_ctl 会返回 EEXIST 错误
 */
Code EventEpoll::Add(int fd, int evt) { /*{{{*/
  if (fd < 0) return kInvalidParam;

  struct epoll_event event;
  event.events = evt;
  event.data.fd = fd;

  int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
  if (ret == -1) return kEpollCtlFailed;

  return kOk;
} /*}}}*/

/**
 * @brief 修改文件描述符监听的事件类型
 * @param fd 文件描述符
 * @param evt 新的事件类型
 * @return kOk 成功；kEpollCtlFailed 修改失败
 */
Code EventEpoll::Mod(int fd, int evt) { /*{{{*/
  if (fd < 0) return kInvalidParam;

  struct epoll_event event;
  event.events = evt;
  event.data.fd = fd;

  int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
  if (ret == -1) return kEpollCtlFailed;

  return kOk;
} /*}}}*/

/**
 * @brief 从事件循环中删除文件描述符
 * @param fd 文件描述符
 * @return kOk 成功；kEpollCtlFailed 删除失败
 *
 * 注意：Linux 2.6.9+ 版本，event 参数可以为 NULL
 */
Code EventEpoll::Del(int fd) { /*{{{*/
  if (fd < 0) return kInvalidParam;

  struct epoll_event event;
  int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &event);
  if (ret == -1) return kEpollCtlFailed;

  return kOk;
} /*}}}*/

void EventEpoll::Print() {}

}  // namespace base
