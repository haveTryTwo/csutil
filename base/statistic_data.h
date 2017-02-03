// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STATISTIC_DATA_H_
#define BASE_STATISTIC_DATA_H_

#include <map>
#include <string>

#include <stdint.h>
#include <sys/time.h>

#include "base/mutex.h"
#include "base/status.h"
#include "base/common.h"


namespace base
{

struct DataInfo
{
    std::string data_type;
    uint64_t total_size;
    uint64_t max_size;
    uint64_t min_size;
    uint64_t avg_size;
    uint64_t total_count;

    DataInfo();
};

class DataStatistic
{
public:
    DataStatistic(const std::string path, int max_file_size=kDefaultStatFileSize);
    ~DataStatistic();

public:
    Code AddStat(const std::string &data_type, uint64_t size);
    Code DumpStat();

private:
    Code FormatWrite(FILE *fp, const std::map<std::string, DataInfo> &data_stats);
    Code CheckIfNeedTransfer(bool *is_need_transfer);
    Code TransferFile();

private:
    Mutex mu_;
    std::string path_;
    int max_file_size_;
    time_t last_dump_time_;
    std::map<std::string, DataInfo> data_stats_;
};

}

#endif
