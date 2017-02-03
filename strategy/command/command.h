// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STRATEGY_COMMAND_H_
#define STRATEGY_COMMAND_H_

#include "base/status.h"

namespace strategy
{

enum CmdStatus
{
    kContinue   = 5,
    kFinish     = 0,
};

class Command
{
    public:
        Command() {}
        virtual ~Command() {}

    public:
        virtual base::Code Excute(uint64_t *cmd_id) = 0;
};

}

#endif
