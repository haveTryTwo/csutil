// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_POLL_H_
#define BASE_EVENT_POLL_H_

#include <poll.h>

#include "base/event.h"
#include "base/status.h"

namespace base
{

class EventPoll : public Event
{/*{{{*/
    public:
        EventPoll();
        ~EventPoll();

    public:
        virtual Code Create(int nfds);
        virtual Code Wait(int time_out_ms);
        virtual Code GetEvents(int *fd, int *evt);

        virtual Code Add(int fd, int evt);
        virtual Code Mod(int fd, int evt);
        virtual Code Del(int fd);
        
        virtual void Print();

    private:
        Code Balance();

    private:
        struct pollfd *pfds_;
        int all_num_of_pfds_;
        int used_num_of_pfds_;
        int del_num_of_pfds_;

        int evt_num_;               // A positive number returned by poll()
        int cur_evt_num_;           // This is number of positive fds
        int cur_idx_of_pfds_;       // This is index of pfd array
};/*}}}*/

}

#endif
