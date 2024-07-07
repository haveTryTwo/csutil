// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STATISTIC_H_
#define BASE_STATISTIC_H_

#include <stdint.h>
#include <sys/time.h>

#include <map>
#include <string>

#include "base/common.h"
#include "base/mutex.h"
#include "base/status.h"

namespace base {

struct StatInfo {
  std::string model;
  int ret_code;
  int total_count;               // count of same model and ret_code
  uint64_t max_time;             // us as time uint
  uint64_t min_time;             // us as time uint
  uint64_t total_time;           // us as time uint
  int time_out_level_one_num;    // number that time larger than kTimeOutLevelOne
  int time_out_level_two_num;    // number that time larger than kTimeOutLevelTwo
  int time_out_level_three_num;  // number that time larger than kTimeOutLevelThree
  int64_t total_recv_flow;       // the total flow bytes that has been received
  int64_t total_send_flow;       // the total flow bytes that has been sended

  StatInfo();
};

class Statistic {
 public:
  Statistic(const std::string path, int max_file_size = kDefaultStatFileSize, int level_one = kTimeOutLevelOne,
            int level_two = kTimeOutLevelTwo, int level_three = kTimeOutLevelThree);

  ~Statistic();

 public:
  Code AddStat(const std::string &model, int ret_code, timeval start, timeval end, int recv_flow, int send_flow,
               int stat_count);
  Code DumpStat();

 public:
  void Print();

 private:
  Code FormatWrite(FILE *fp, const std::map<std::pair<std::string, int>, StatInfo> &stat_list);
  Code CheckIfNeedTransfer(bool *is_need_transfer);
  Code TransferFile();

 private:
  std::string path_;
  int max_file_size_;
  Mutex mu_;
  int time_out_level_one_;    // milliseconds
  int time_out_level_two_;    // milliseconds
  int time_out_level_three_;  // milliseconds
  time_t last_dump_time_;
  std::map<std::pair<std::string, int>, StatInfo> stat_list_;
};

}  // namespace base

#endif
