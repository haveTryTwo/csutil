// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "command.h"

namespace strategy
{

base::Code Invoker()
{
    uint64_t cmd_id = 0;
    TimerCommandQueue timer_cmd_queue;
    for (int i = 0; i < 10; ++i)
    {
        Command *cmd = new TimerCommand(cmd_id);
        base::Code ret = cmd->Excute();
        if (ret == kContinue)
        {
            timer_cmd_queue->Set(cmd_id, cmd);
        }
        else
        {
            fprintf(stdout, "Cmd:%llu finish, ret:%d\n", cmd_id, ret);
        }
        ++cmd_id;
    }

    // CallBack
    uint64_t cmd_id = ;
    Command *cmd = NULL;
    base::Code ret = timer_cmd_queue->Get(cmd_id, &cmd);
    cmd->Excute();
    if (ret == kContinue)
    {
        timer_cmd_queue->Set(cmd_id, cmd);
    }
}

}

#ifdef _INVOKER_MAIN_TEST_
int main(int argc, char *argv[])
{
    return 0;
}
#endif
