// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <unistd.h>

#include "base/event_epoll.h"

namespace base
{

EventEpoll::EventEpoll() : epfd_(-1), evts_(NULL), 
    max_evts_num_(0), cur_evts_pos_(0), evts_num_(0)
{
}

EventEpoll::~EventEpoll()
{/*{{{*/
    if (epfd_ != -1)
    {
        close(epfd_);
        epfd_ = -1;
    }

    if (evts_ != NULL)
    {
        delete []evts_;
        evts_ = NULL;
    }
}/*}}}*/

Code EventEpoll::Create(int nfds)
{/*{{{*/
    if (nfds <= 0) return kInvalidParam;

    epfd_ = epoll_create(nfds);
    if (epfd_ == -1) return kEpollCreateFailed;

    evts_ = new struct epoll_event[nfds];
    if (evts_ == NULL)
    {
        close(epfd_);
        epfd_ = -1;
        return kEpollFailed;
    }
    max_evts_num_ = nfds;

    return kOk;
}/*}}}*/

Code EventEpoll::Wait(int time_out_ms)
{/*{{{*/
    while (true)
    {
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
}/*}}}*/

Code EventEpoll::GetEvents(int *fd, int *evt)
{/*{{{*/
    if (cur_evts_pos_ < evts_num_)
    {
        *evt = evts_[cur_evts_pos_].events;
        *fd = evts_[cur_evts_pos_].data.fd;
        ++cur_evts_pos_;
        return kOk;
    }

    return kNotFound;
}/*}}}*/

Code EventEpoll::Add(int fd, int evt)
{/*{{{*/
    struct epoll_event event;
    event.events = evt;
    event.data.fd = fd;
    
    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
    if (ret == -1) return kEpollCtlFailed;

    return kOk;
}/*}}}*/

Code EventEpoll::Mod(int fd, int evt)
{/*{{{*/
    struct epoll_event event;
    event.events = evt;
    event.data.fd = fd;
    
    int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
    if (ret == -1) return kEpollCtlFailed;

    return kOk;
}/*}}}*/

Code EventEpoll::Del(int fd)
{/*{{{*/
    struct epoll_event event;
    
    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &event);
    if (ret == -1) return kEpollCtlFailed;

    return kOk;
}/*}}}*/

void EventEpoll::Print()
{
}

}
