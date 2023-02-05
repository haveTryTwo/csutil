// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <sys/stat.h>

#include "statistic_data.h"

namespace base
{

DataInfo::DataInfo() : data_type(""), total_size(0), max_size(0),
    min_size(0), avg_size(0), total_count(0)
{/*{{{*/
}/*}}}*/

DataStatistic::DataStatistic(const std::string path, int max_file_size) : 
    path_(path), max_file_size_(max_file_size)
{/*{{{*/
    time(&last_dump_time_);
}/*}}}*/

DataStatistic::~DataStatistic()
{/*{{{*/
}/*}}}*/

Code DataStatistic::AddStat(const std::string &data_type, uint64_t size)
{/*{{{*/
    MutexLock mlock(&mu_);

    std::map<std::string, DataInfo>::iterator it = data_stats_.find(data_type);
    if (it == data_stats_.end())
    {
        DataInfo tmp_info;
        tmp_info.data_type = data_type;
        tmp_info.total_size = size;
        tmp_info.max_size = size;
        tmp_info.min_size = size;
        tmp_info.avg_size = size;
        tmp_info.total_count = 1;

        data_stats_.insert(std::pair<std::string, DataInfo>(data_type, tmp_info));
    }
    else
    {
        it->second.total_size += size;
        if (it->second.max_size < size) it->second.max_size = size;
        if (it->second.min_size > size) it->second.min_size = size;
        it->second.total_count += 1;
        it->second.avg_size = it->second.total_size / it->second.total_count;
    }

    return kOk;
}/*}}}*/

Code DataStatistic::DumpStat()
{/*{{{*/
    bool is_need_transfer = false;
    Code ret = CheckIfNeedTransfer(&is_need_transfer);
    if (ret != kOk) return ret;

    if (is_need_transfer)
    {
        ret = TransferFile();
        if (ret != kOk) return ret;
    }

    FILE *fp = fopen(path_.c_str(), "a+");
    if (fp == NULL) return kOpenError;

    time_t cur_time;
    time(&cur_time);
    struct tm cur_time_tm;
    localtime_r(&cur_time, &cur_time_tm);
    int diff_dump_time = cur_time - last_dump_time_;
    last_dump_time_ = cur_time;

    fprintf(fp, "\n\n\n%4d/%02d/%02d %02d:%02d:%02d - dump %d seconds data stat report info:\n",
                 cur_time_tm.tm_year+1900, cur_time_tm.tm_mon+1, cur_time_tm.tm_mday,
                 cur_time_tm.tm_hour, cur_time_tm.tm_min, cur_time_tm.tm_sec,
                 diff_dump_time);

    std::map<std::string, DataInfo> data_stats_tmp;
    {
        MutexLock mlock(&mu_);
        data_stats_tmp = data_stats_;
        data_stats_.clear();
    }

    FormatWrite(fp, data_stats_tmp);

    fflush(fp);
    fclose(fp);

    return kOk;
}/*}}}*/

Code DataStatistic::FormatWrite(FILE *fp, const std::map<std::string, DataInfo> &data_stats)
{/*{{{*/
    if (fp == NULL) return kInvalidParam; 

    fprintf(fp, "|%10s|%14s|%14s|%14s|%14s|%14s|\n",
            "DATA_TYPE", "TOTAL_SIZE", "MAX_SIZE", "MIN_SIZE", "AVG_SIZE", "TOTAL_COUNT");

    std::map<std::string, DataInfo>::const_iterator it = data_stats.begin();
    for (; it != data_stats.end(); ++it)
    {
        if (it->second.total_count <= 0) continue;

        fprintf(fp, "|%10s|%14llu|%14llu|%14llu|%14llu|%14llu|\n",
                it->second.data_type.c_str(),
                (unsigned long long)it->second.total_size,
                (unsigned long long)it->second.max_size,
                (unsigned long long)it->second.min_size,
                (unsigned long long)it->second.avg_size,
                (unsigned long long)it->second.total_count
               );
    }

    return kOk;
}/*}}}*/

Code DataStatistic::CheckIfNeedTransfer(bool *is_need_transfer)
{/*{{{*/
    if (is_need_transfer == NULL) return kInvalidParam;

    *is_need_transfer = false;
    struct stat st;
    int ret = stat(path_.c_str(), &st);
    if (ret == -1)
    {
        if (errno != ENOENT) return kStatFailed;
    }
    else 
    {
        if (st.st_size >= max_file_size_) *is_need_transfer = true;
    }

    return kOk;
}/*}}}*/

Code DataStatistic::TransferFile()
{/*{{{*/
    std::string backup_path = path_ + kBackupSuffix;
    int ret = rename(path_.c_str(), backup_path.c_str());
    if (ret == -1) return kRenameFailed;

    return kOk;
}/*}}}*/

}
