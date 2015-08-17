// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/event_loop.h"

namespace base
{

EventLoop::EventLoop() : actions_(), evt_(NULL)
{/*{{{*/
}/*}}}*/

EventLoop::~EventLoop()
{/*{{{*/
    actions_.clear();

    if (evt_ != NULL)
    {
        delete evt_;        
        evt_ = NULL;
    }
}/*}}}*/

Code EventLoop::Init(EventType evt_type)
{/*{{{*/
    Code ret = kOk;
    switch (evt_type)
    {
        case kSelect:
            break;
        case kPoll:
            evt_ = new EventPoll();
            ret = evt_->Create(kDefaultSizeOfFds);
            break;
        case kEPoll:
            break;
        default:
            evt_ = new EventPoll();
            ret = evt_->Create(kDefaultSizeOfFds);
            break;
    }

    return ret;
}/*}}}*/

Code EventLoop::Add(int fd, int evt, EventFunc func, void *param)
{/*{{{*/
    Code ret = evt_->Add(fd, evt);
    if (ret != kOk) return ret;
  
    EventItem item = { fd, evt, func, param };
    actions_.insert(std::make_pair<int, EventItem>(fd, item));

    return ret;
}/*}}}*/

Code EventLoop::Mod(int fd, int evt, EventFunc func, void *param)
{/*{{{*/
    Code ret = evt_->Mod(fd, evt);
    if (ret != kOk) return ret;
  
    EventItem &item = actions_[fd];
    item.fd = fd;
    item.evt = evt;
    item.func = func;
    item.param = param;

    return ret;
}/*}}}*/

Code EventLoop::Del(int fd)
{/*{{{*/
    actions_.erase(fd);

    Code ret = evt_->Del(fd);
    return ret;
}/*}}}*/

Code EventLoop::Run()
{/*{{{*/
    Code ret = kOk;

    while (true)
    {
        ret = evt_->Wait(kDefaultWaitTimeMs);
        if (ret != kOk)
            continue;

        while (true)
        {
            int fd;
            int evt;
            ret = evt_->GetEvents(&fd, &evt);
            if (ret != kOk)
                break;

            typedef std::map<int, EventItem>::iterator Iter;
            Iter it = actions_.find(fd);
            if (it == actions_.end())
                break;

            (it->second).func(fd, evt, (it->second).param);
        }
    }

    return ret;
}/*}}}*/

}

#ifdef _EVENT_LOOP_MAIN_TEST_
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;
    EventLoop main_loop;
    main_loop.Init(kPoll);

    return 0;
}/*}}}*/
#endif
