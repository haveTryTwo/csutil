// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include "timer_command.h"

namespace strategy
{

TimerCommand::TimerCommand(uint64_t cmd_id, uint64_t step_timer_interval, uint64_t total_begin_time, uint64_t total_timer_interval, bool in_queue)  : cmd_id_(cmd_id), step_begin_time_(0), step_end_time_(0),
                          step_timer_interval_(step_timer_interval), total_begin_time_(total_begin_time),
                          total_end_time_(0), total_timer_interval_(total_timer_interval),
                          cur_state_(kBeginState), in_queue_(in_queue)
{/*{{{*/
}/*}}}*/

TimerCommand::~TimerCommand()
{/*{{{*/
}/*}}}*/


base::Code TimerCommand::Init(const std::string &msg)
{/*{{{*/
    msg_ = msg;

    // init key

    return base::kOk;
}/*}}}*/

base::Code TimerCommand::Expire()
{/*{{{*/
    base::LOG_ERR("Timer Command Expire! cmd_id:%llu, step:%u", cmd_id_, cur_state_);

    return base::kOk;
}/*}}}*/

base::Code TimerCommand::SetStepBeginTime(uint64_t step_begin_time)
{/*{{{*/
    step_begin_time_ = step_begin_time;
    return base::kOk;
}/*}}}*/

base::Code TimerCommand::CheckIsExpire(uint64_t cur_time, bool *is_expire)
{/*{{{*/
    if (cur_time < step_begin_time_ || cur_time < total_begin_time_ || is_expire == NULL) 
        return base::kInvalidParam;

    *is_expire = false;
    if (cur_time - step_begin_time_ > step_timer_interval_)
    {
        *is_expire = true;
        return base::kOk;
    }

    if (cur_time - total_begin_time_ > total_timer_interval_)
    {
        *is_expire = true;
        return base::kOk;
    }

    return base::kOk;
}/*}}}*/


base::Code TimerCommand::CheckIsStepExpire(uint64_t cur_time, bool *is_expire)
{/*{{{*/
    if (cur_time < step_begin_time_ || is_expire == NULL) 
        return base::kInvalidParam;

    *is_expire = false;
    if (cur_time - step_begin_time_ > step_timer_interval_)
    {
        *is_expire = true;
        return base::kOk;
    }

    return base::kOk;
}/*}}}*/

base::Code TimerCommand::CheckIsTotalExpire(uint64_t cur_time, bool *is_expire)
{/*{{{*/
    if (cur_time < total_begin_time_ || is_expire == NULL) 
        return base::kInvalidParam;

    *is_expire = false;
    if (cur_time - total_begin_time_ > total_timer_interval_)
    {
        *is_expire = true;
        return base::kOk;
    }

    return base::kOk;
}/*}}}*/

bool TimerCommand::IsInQueue()
{/*{{{*/
    return in_queue_;
}/*}}}*/

base::Code TimerCommand::GetQueueKey(std::string *key)
{/*{{{*/
    if (key == NULL) return base::kInvalidParam;

    *key = key_;

    return base::kOk;
}/*}}}*/

base::Code TimerCommand::Excute(uint64_t *cmd_id)
{/*{{{*/
    if (cmd_id == NULL) return base::kInvalidParam;

    // excute
    base::LOG_INFO("Excute! cmd_id:%llu, cur_state:%u, msg:%s", cmd_id_, cur_state_, msg_.c_str());

    return (base::Code)kContinue;
}/*}}}*/

}
