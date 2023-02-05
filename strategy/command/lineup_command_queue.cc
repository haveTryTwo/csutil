// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/time.h"

#include "lineup_command_queue.h"

namespace strategy
{

LineupCommandQueue::LineupCommandQueue()
{
}

LineupCommandQueue::~LineupCommandQueue()
{
}

base::Code LineupCommandQueue::Check(const std::string &key, bool *is_exist)
{/*{{{*/
    if (is_exist == NULL) return base::kInvalidParam;

    *is_exist = false;
    std::map<std::string, std::deque<TimerCommand*> >::iterator queue_it;
    queue_it = lineup_cmd_queue_.find(key);
    if (queue_it != lineup_cmd_queue_.end() && queue_it->second.size() > 0)
    {
        *is_exist = true;
    }

    return base::kOk;
}/*}}}*/

base::Code LineupCommandQueue::Add(const std::string &key, TimerCommand* cmd, bool *is_first)
{/*{{{*/
    if (cmd == NULL) return base::kInvalidParam;

    if (is_first != NULL) *is_first = false;

    std::map<std::string, std::deque<TimerCommand*> >::iterator queue_it;
    std::pair<std::map<std::string, std::deque<TimerCommand*> >::iterator, bool> insert_pair;
    queue_it = lineup_cmd_queue_.find(key);
    if (queue_it == lineup_cmd_queue_.end())
    {
        if (is_first != NULL) *is_first = true;

        insert_pair = lineup_cmd_queue_.insert(std::pair<std::string, std::deque<TimerCommand*> >(key, std::deque<TimerCommand*>()));
        queue_it = insert_pair.first;
    }
    else if (queue_it->second.size() == 0)
    {
        if (is_first != NULL) *is_first = true;
    }

    queue_it->second.push_back(cmd);

    return base::kOk;
}/*}}}*/

base::Code LineupCommandQueue::GoNext(const std::string &key, TimerCommand **cmd)
{/*{{{*/
    if (cmd == NULL) return base::kInvalidParam;
    *cmd = NULL;

    std::map<std::string, std::deque<TimerCommand*> >::iterator queue_it;
    queue_it = lineup_cmd_queue_.find(key);
    if (queue_it == lineup_cmd_queue_.end())
    {
        return (base::Code)kFinish;
    }
    else if (queue_it->second.size() == 0)
    {
        lineup_cmd_queue_.erase(queue_it);
        return (base::Code)kFinish;
    }

    base::Code ret = (base::Code)kContinue;
    queue_it->second.pop_front();

    std::deque<TimerCommand*>::iterator wait_it;
    while (queue_it->second.size() > 0)
    {/*{{{*/
        wait_it = queue_it->second.begin();

        struct timeval tm;
        base::Time::GetTime(&tm);
        bool is_expire = false;
        ret = (*wait_it)->CheckIsTotalExpire(tm.tv_sec, &is_expire);
        if (ret != base::kOk)
        {
            base::LOG_ERR("Failed to check total expire of key:%s, ret:%d", key.c_str(), ret);
            delete *wait_it;
            queue_it->second.pop_front();
            continue;
        }

        if (is_expire)
        {
            base::LOG_ERR("Queue expire of key:%s", key.c_str());
            delete *wait_it;
            queue_it->second.pop_front();
            continue;
        }

        break;
    }/*}}}*/

    if (queue_it->second.size() == 0)
    {
        lineup_cmd_queue_.erase(queue_it);
        return (base::Code)kFinish;
    }

    wait_it = queue_it->second.begin();
    *cmd = *wait_it;
    return (base::Code)kContinue;
}/*}}}*/

}
