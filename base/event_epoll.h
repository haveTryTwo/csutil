// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_EPOLL_H_
#define BASE_EVENT_EPOLL_H_

#include <sys/epoll.h>

#include "base/event.h"
#include "base/status.h"

namespace base
{

class EventEpoll : public Event
{
    public:
        EventEpoll();
        virtual ~EventEpoll();

    public:
        virtual Code Create(int nfds);
        virtual Code Wait(int time_out_ms);
        virtual Code GetEvents(int *fd, int *evt);

        virtual Code Add(int fd, int evt);
        virtual Code Mod(int fd, int evt);
        virtual Code Del(int fd);

        virtual void Print();

    private:
        int epfd_;
        struct epoll_event *evts_;
        int max_evts_num_;
        int cur_evts_pos_;
        int evts_num_;
};
    
}

#endif
