// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdint.h>

#include "base/status.h"

#include "command.h"

namespace strategy
{

class TimerCommand : public Command
{/*{{{*/
    public:
        enum State
        {
            kBeginState = 0,
        };

    public:
        TimerCommand(uint64_t cmd_id, uint64_t step_timer_interval, uint64_t total_begin_time, uint64_t total_timer_interval, bool in_queue);
        virtual ~TimerCommand();

    public:
        virtual base::Code Init(const std::string &msg);
        virtual base::Code Expire();

        base::Code SetStepBeginTime(uint64_t step_begin_time);
        base::Code CheckIsExpire(uint64_t cur_time, bool *is_expire);
        base::Code CheckIsStepExpire(uint64_t cur_time, bool *is_expire);
        base::Code CheckIsTotalExpire(uint64_t cur_time, bool *is_expire);

        bool IsInQueue();
        base::Code GetQueueKey(std::string *key);

        virtual base::Code Excute(uint64_t *cmd_id);

    private:
        uint64_t cmd_id_;

        uint64_t step_begin_time_;
        uint64_t step_end_time_;
        uint64_t step_timer_interval_;

        uint64_t total_begin_time_;
        uint64_t total_end_time_;
        uint64_t total_timer_interval_;

        uint32_t cur_state_;

        std::string msg_;

        std::string key_;
        bool in_queue_;
};/*}}}*/

}
