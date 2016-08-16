// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/time.h"

#include "timer_command_queue.h"

namespace strategy
{

TimerCommandQueue::TimerCommandQueue()
{
}

TimerCommandQueue::~TimerCommandQueue()
{
}

base::Code TimerCommandQueue::Set(uint64_t cmd_id, uint64_t step_begin_time, TimerCommand* cmd)
{/*{{{*/
    if (cmd == NULL) return base::kInvalidParam;

    std::map<uint64_t, TimerCommand*>::iterator timer_it;
    timer_it = timer_cmd_queue_.find(cmd_id);
    if (timer_it != timer_cmd_queue_.end())
    {
        base::LOG_ERR("Exist of cmd_id:%llu", cmd_id);
        return base::kExist;
    }

    cmd->SetStepBeginTime(step_begin_time);
    timer_cmd_queue_.insert(std::make_pair<uint64_t, TimerCommand*>(cmd_id, cmd));

    return base::kOk;
}/*}}}*/

base::Code TimerCommandQueue::Get(uint64_t cmd_id, TimerCommand **cmd)
{/*{{{*/
    if (cmd == NULL) return base::kInvalidParam;

    std::map<uint64_t, TimerCommand*>::iterator timer_it;
    timer_it = timer_cmd_queue_.find(cmd_id);
    if (timer_it == timer_cmd_queue_.end())
    {
        base::LOG_ERR("Failed to find cmd of cmd_id:%llu", cmd_id);
        return base::kNotFound;
    }

    *cmd = timer_it->second;
    timer_cmd_queue_.erase(timer_it);

    return base::kOk;
}/*}}}*/

base::Code TimerCommandQueue::Expire()
{/*{{{*/
    base::Code ret = base::kOk;
    std::map<uint64_t, TimerCommand*>::iterator timer_it;
    for (timer_it = timer_cmd_queue_.begin(); timer_it != timer_cmd_queue_.end(); ++timer_it)
    {
        struct timeval tm;
        base::Time::GetTime(&tm);
        bool is_expire = false;
        ret = timer_it->second->CheckIsExpire(tm.tv_sec, &is_expire);
        if (ret != base::kOk)
        {
            base::LOG_ERR("Failed to check expire of cmd_id:%llu, ret:%d", timer_it->first, ret);
            // delete the timer_cmd and erase it in the queue
            delete timer_it->second;
            timer_cmd_queue_.erase(timer_it++);
            continue;
        }
        
        if (is_expire)
        {
            timer_it->second->Expire();

            // delete the timer_cmd and erase it in the queue
            delete timer_it->second;
            timer_cmd_queue_.erase(timer_it++);
            continue;
        }
    }

    return base::kOk;
}/*}}}*/

}
