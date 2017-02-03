// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include "timer_command.h"

namespace strategy
{

class TimerCommandQueue
{
    public:
        TimerCommandQueue();
        ~TimerCommandQueue();

    public:
        base::Code Set(uint64_t cmd_id, uint64_t step_begin_time, TimerCommand* cmd);
        base::Code Get(uint64_t cmd_id, TimerCommand **cmd);
        base::Code Expire();

    private:
        std::map<uint64_t, TimerCommand*> timer_cmd_queue_;
};

}
