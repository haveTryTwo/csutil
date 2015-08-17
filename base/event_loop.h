// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_LOOP_H_
#define BASE_EVENT_LOOP_H_

#include <map>

#include "base/event.h"
#include "base/common.h"

namespace base
{

typedef Code (*EventFunc)(int fd, int evt, void *p);

struct EventItem
{/*{{{*/
    int fd;
    int evt;
    EventFunc func;
    void *param;
};/*}}}*/

class EventLoop
{/*{{{*/
    public:
        EventLoop();
        ~EventLoop();

        Code Init(EventType evt_type);

    public:
        Code Add(int fd, int evt, EventFunc func, void *param);
        Code Mod(int fd, int evt, EventFunc func, void *param);
        Code Del(int fd);

    public:
        Code Run();

    private:
        EventLoop(const EventLoop &el);
        EventLoop& operator= (const EventLoop &el);

    private:
        std::map<int, EventItem> actions_;
        Event *evt_;
};/*}}}*/

}

#endif
