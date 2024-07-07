// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/load_ctrl.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "base/common.h"

namespace base {

LoadCtrl::LoadCtrl() { /*{{{*/
  total_time_spin_ms_ = 0;
  unit_time_spin_ms_ = 0;
  max_num_of_all_spin_ = 0;

  grids_num_ = 0;
  grids_ = NULL;

  cur_grid_idx_ = 0;
  cur_num_of_all_spin_ = 0;

  gettimeofday(&start_time_, NULL);
} /*}}}*/

LoadCtrl::LoadCtrl(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin) : mu_() { /*{{{*/
  total_time_spin_ms_ = total_time_spin_ms;
  unit_time_spin_ms_ = unit_time_spin_ms;
  max_num_of_all_spin_ = max_num_of_all_spin;

  grids_num_ = 0;
  grids_ = NULL;

  cur_grid_idx_ = 0;
  cur_num_of_all_spin_ = 0;

  gettimeofday(&start_time_, NULL);
} /*}}}*/

LoadCtrl::~LoadCtrl() { /*{{{*/
  if (grids_ != NULL) {
    delete[] grids_;
    grids_ = NULL;
  }
} /*}}}*/

Code LoadCtrl::Init() { /*{{{*/
  if (total_time_spin_ms_ <= 0 || unit_time_spin_ms_ <= 0 || max_num_of_all_spin_ <= 0) return kInvalidParam;

  grids_num_ = total_time_spin_ms_ / unit_time_spin_ms_;
  if (total_time_spin_ms_ % unit_time_spin_ms_) ++grids_num_;

  grids_ = new int[grids_num_];
  assert(grids_ != NULL);
  memset(grids_, 0, sizeof(int) * grids_num_);

  return kOk;
} /*}}}*/

Code LoadCtrl::Init(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin) { /*{{{*/
  return Modify(total_time_spin_ms, unit_time_spin_ms, max_num_of_all_spin);
} /*}}}*/

Code LoadCtrl::Modify(int total_time_spin_ms, int unit_time_spin_ms, int max_num_of_all_spin) { /*{{{*/
  total_time_spin_ms_ = total_time_spin_ms;
  unit_time_spin_ms_ = unit_time_spin_ms;
  max_num_of_all_spin_ = max_num_of_all_spin;

  if (grids_ != NULL) {
    delete grids_;
    grids_ = NULL;
  }

  cur_grid_idx_ = 0;
  cur_num_of_all_spin_ = 0;

  gettimeofday(&start_time_, NULL);

  return Init();
} /*}}}*/

Code LoadCtrl::CheckFlow(const timeval &now, bool *is_restrict) { /*{{{*/
  if (is_restrict == NULL) return kInvalidParam;
  *is_restrict = false;

  // If these members are less than or equal 0, this means that there is no
  // need to check the flow
  if (total_time_spin_ms_ <= 0 || unit_time_spin_ms_ <= 0 || max_num_of_all_spin_ <= 0) return kOk;

  MutexLock mlock(&mu_);
  Code ret = UpdateGrids(now);
  if (ret != kOk) return ret;

  if (cur_num_of_all_spin_ > max_num_of_all_spin_) {
    *is_restrict = true;
    return kOk;
  }

  grids_[cur_grid_idx_]++;
  cur_num_of_all_spin_++;

  return kOk;
} /*}}}*/

Code LoadCtrl::SetMaxFlow(int max_flow) { /*{{{*/
  max_num_of_all_spin_ = max_flow;
  return kOk;
} /*}}}*/

Code LoadCtrl::GetLoadInfo(int *total_time_spin_ms, int *cur_num_of_all_spin) { /*{{{*/
  if (total_time_spin_ms != NULL) *total_time_spin_ms = total_time_spin_ms_;
  if (cur_num_of_all_spin != NULL) *cur_num_of_all_spin = cur_num_of_all_spin_;

  return kOk;
} /*}}}*/

Code LoadCtrl::UpdateGrids(const timeval &now) { /*{{{*/
  int move_us = (now.tv_sec - start_time_.tv_sec) * kMillion + (now.tv_usec - start_time_.tv_usec);

  // If the elapsed time has exceeded the time spin, then recheck
  if (move_us < 0 || move_us >= total_time_spin_ms_ * kThousand) {
    memset(grids_, 0, sizeof(int) * grids_num_);
    cur_grid_idx_ = 0;
    cur_num_of_all_spin_ = 0;
    gettimeofday(&start_time_, NULL);
    return kOk;
  }

  // If the elapsed time has exceeded one time spin
  int unit_time_spin_us = unit_time_spin_ms_ * kThousand;
  if (move_us > unit_time_spin_us) {
    int move_num = move_us / (unit_time_spin_us);
    for (int i = 0; i < move_num; ++i) {
      cur_grid_idx_ = (cur_grid_idx_ + 1) % grids_num_;
      cur_num_of_all_spin_ -= grids_[cur_grid_idx_];
      grids_[cur_grid_idx_] = 0;
    }

    start_time_.tv_usec += move_num * unit_time_spin_us;
    if (start_time_.tv_usec >= kMillion) {
      start_time_.tv_sec += start_time_.tv_usec / kMillion;
      start_time_.tv_usec = start_time_.tv_usec % kMillion;
    }
  } else {
  }

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _LOAD_CTRL_MAIN_TEST_
#include <unistd.h>
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  int total_time_spin_ms = 1000;
  int unit_time_spin_ms = 50;
  int max_num_of_all_spin = 10000;
  bool is_restrict = false;
  timeval now;

  LoadCtrl loadCtrl(total_time_spin_ms, unit_time_spin_ms, max_num_of_all_spin);
  loadCtrl.Init();

  for (int i = 0; i < 10022; ++i) {
    gettimeofday(&now, NULL);
    Code ret = loadCtrl.CheckFlow(now, &is_restrict);
    assert(ret == kOk);

    if (is_restrict) fprintf(stderr, "Now restrict! i:%d\n", i);
    usleep(10);
  }

  return 0;
} /*}}}*/
#endif
