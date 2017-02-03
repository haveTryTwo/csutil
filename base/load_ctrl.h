// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOAD_CTRL_H_
#define BASE_LOAD_CTRL_H_

#include "base/mutex.h"
#include "base/status.h"

namespace base
{

/**
 *
 */
class LoadCtrl
{/*{{{*/
    public:
        LoadCtrl();
        LoadCtrl(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin);
        ~LoadCtrl();

        Code Init();
        Code Init(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin);

        Code Modify(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin);

        Code CheckFlow(const timeval &now, bool *isRestrict);
        Code SetMaxFlow(int max_flow);

        Code GetLoadInfo(int *total_time_spin_ms, int *cur_num_of_all_spin);

    private:
        Code UpdateGrids(const timeval &now);

    private:
        int total_time_spin_ms_;
        int unit_time_spin_ms_;
        int max_num_of_all_spin_;

        int grids_num_;
        int *grids_;
        int cur_grid_idx_;
        int cur_num_of_all_spin_;
        timeval start_time_;

        Mutex mu_;
};/*}}}*/

}

#endif
