// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <deque>
#include <string>

#include "command.h"
#include "timer_command.h"

namespace strategy
{

/**
 * Aim to line up the request, only one request has finished, next request can be called
 */
class LineupCommandQueue
{
    public:
        LineupCommandQueue();
        ~LineupCommandQueue();

        base::Code Check(const std::string &key, bool *is_exist);
        base::Code Add(const std::string &key, TimerCommand* cmd, bool *is_first);
        base::Code GoNext(const std::string &key, TimerCommand **cmd);

    private:
        std::map<std::string, std::deque<TimerCommand*> > lineup_cmd_queue_;
};

}
