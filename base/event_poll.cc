// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include "base/event_poll.h"

namespace base
{

EventPoll::EventPoll()
{/*{{{*/
    pfds_ = NULL;
    all_num_of_pfds_ = 0;
    used_num_of_pfds_ = 0;
    del_num_of_pfds_ = 0;

    evt_num_ = 0;
    cur_evt_num_ = 0;
}/*}}}*/

EventPoll::~EventPoll()
{/*{{{*/
    if (pfds_ != NULL)
    {
        delete []pfds_;
        pfds_ = NULL;
    }
}/*}}}*/

Code EventPoll::Create(int nfds)
{/*{{{*/
    if (nfds <= 0) return kInvalidParam;

    all_num_of_pfds_ = nfds;
    pfds_ = new struct pollfd[nfds];
    assert(pfds_ != NULL);
    memset(pfds_, -1, sizeof(struct pollfd)*nfds);

    return kOk;
}/*}}}*/

Code EventPoll::Wait(int time_out_ms)
{/*{{{*/
    if (del_num_of_pfds_ > 0) Balance();

    while (true)
    {
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
}/*}}}*/

Code EventPoll::GetEvents(int *fd, int *evt)
{/*{{{*/
    if (fd == NULL || evt == NULL) return kInvalidParam;

    if (evt_num_ <= 0) return kNotFound;

    while (cur_idx_of_pfds_ < used_num_of_pfds_ && cur_evt_num_ < evt_num_)
    {
        if (pfds_[cur_idx_of_pfds_].fd != -1 && pfds_[cur_idx_of_pfds_].revents != 0)
        {
            *fd = pfds_[cur_idx_of_pfds_].fd;
            *evt = pfds_[cur_idx_of_pfds_].revents;
            ++cur_idx_of_pfds_;
            ++cur_evt_num_;
            return kOk;
        }
        ++cur_idx_of_pfds_;
    }

    return kNotFound;
}/*}}}*/

Code EventPoll::Add(int fd, int evt)
{/*{{{*/
    // TODO: if there are deleted space, may reblance first
    
    if (used_num_of_pfds_ >= all_num_of_pfds_) return kFull;

    pfds_[used_num_of_pfds_].fd = fd;
    pfds_[used_num_of_pfds_].events = evt;
    pfds_[used_num_of_pfds_].revents = 0;

    used_num_of_pfds_++;

    return kOk;
}/*}}}*/

Code EventPoll::Mod(int fd, int evt)
{/*{{{*/
    for (int i = 0; i < used_num_of_pfds_; ++i)
    {
        if (pfds_[i].fd == fd)
        {
            pfds_[i].events = evt;
            return kOk;
        }
    }

    return kNotFound;
}/*}}}*/

Code EventPoll::Del(int fd)
{/*{{{*/
    for (int i = 0; i < used_num_of_pfds_; ++i)
    {
        if (pfds_[i].fd == fd)
        {
            pfds_[i].fd = -1;
            ++del_num_of_pfds_;
            return kOk;
        }
    }

    return kNotFound;
}/*}}}*/

void EventPoll::Print()
{/*{{{*/
    fprintf(stderr, "\033[1;31m%10s | %10s | %10s\033[0m\n", "fd", "events", "revents");
    for (int i = 0; i < used_num_of_pfds_; ++i)
    {
        fprintf(stderr, "%10d | %10d | %10d\n", 
                pfds_[i].fd, pfds_[i].events, pfds_[i].revents);
    }
}/*}}}*/

Code EventPoll::Balance()
{/*{{{*/
    int start_pos = 0;
    int end_pos = used_num_of_pfds_ - 1;

    while (start_pos < end_pos)
    {
        while (start_pos < end_pos && pfds_[start_pos].fd != -1)
            ++start_pos;

        while (start_pos < end_pos && pfds_[end_pos].fd == -1)
            --end_pos;

        Swap(pfds_+start_pos, pfds_+end_pos);
    }

    used_num_of_pfds_ -= del_num_of_pfds_;
    del_num_of_pfds_ = 0;

    return kOk;
}/*}}}*/

}

#ifdef _EVENT_POLL_MAIN_TEST_
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base; 

    Event *ev = new EventPoll();
    ev->Create(100);
    ev->Print();

    // int time_out_ms = 1000;

    for (int i = 0;  i < 7; ++i)
    {
        ev->Add(i, POLLIN | POLLOUT);
    }
    ev->Print();

    ev->Del(2);
    ev->Del(4);
    ev->Del(22);
    ev->Print();

    return 0;
}/*}}}*/
#endif
