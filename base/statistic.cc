// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#include "base/common.h"
#include "base/statistic.h"

namespace base
{

StatInfo::StatInfo() : model(""), ret_code(0), total_count(0), max_time(0), min_time(0), total_time(0), 
    time_out_level_one_num(0), time_out_level_two_num(0), time_out_level_three_num(0),
    total_recv_flow(0), total_send_flow(0)
{
}


Statistic::Statistic(const std::string path, int max_file_size,
        int level_one, int level_two, int level_three) :
        path_(path), max_file_size_(max_file_size), mu_(),
        time_out_level_one_(level_one), 
        time_out_level_two_(level_two),
        time_out_level_three_(level_three)
{
    time(&last_dump_time_);
}

Statistic::~Statistic()
{
}

Code Statistic::AddStat(const std::string &model, int ret_code, timeval start, timeval end,
                        int recv_flow, int send_flow, int stat_count)
{/*{{{*/
    int64_t diff_time = (end.tv_sec-start.tv_sec)*kUnitConvOfMicrosconds + (end.tv_usec-start.tv_usec);
    if (diff_time < 0) diff_time = -diff_time;

    MutexLock mlock(&mu_);

    std::pair<std::string, int> key(model, ret_code);
    std::map<std::pair<std::string, int>, StatInfo>::iterator it = stat_list_.find(key);
    if (it != stat_list_.end())
    {
        it->second.total_count += stat_count;
        if ((it->second.max_time) < (uint64_t)diff_time) it->second.max_time = diff_time;
        if ((it->second.min_time) > (uint64_t)diff_time) it->second.min_time = diff_time;
        it->second.total_time += diff_time;
        if (diff_time >= time_out_level_three_*1000) it->second.time_out_level_three_num++;
        else if (diff_time >= time_out_level_two_*1000) it->second.time_out_level_two_num++;
        else if (diff_time >= time_out_level_one_*1000) it->second.time_out_level_one_num++;
        it->second.total_recv_flow += recv_flow;
        it->second.total_send_flow += send_flow;
        return kOk;
    }

    StatInfo stat_info;
    stat_info.model.assign(model);
    stat_info.ret_code = ret_code;
    stat_info.total_count = stat_count;
    stat_info.max_time = stat_info.min_time = diff_time;
    stat_info.total_time = diff_time;
    if (diff_time >= time_out_level_three_*1000) stat_info.time_out_level_three_num++;
    else if (diff_time >= time_out_level_two_*1000) stat_info.time_out_level_two_num++;
    else if (diff_time >= time_out_level_one_*1000) stat_info.time_out_level_one_num++;
    stat_info.total_recv_flow = recv_flow;
    stat_info.total_send_flow = send_flow;
    stat_list_.insert(std::pair<std::pair<std::string, int>, StatInfo>(key, stat_info));

    return kOk;
}/*}}}*/

Code Statistic::DumpStat()
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

    fprintf(fp, "\n\n\n%4d/%02d/%02d %02d:%02d:%02d - dump %d seconds stat info:\n",
                 cur_time_tm.tm_year+1900, cur_time_tm.tm_mon+1, cur_time_tm.tm_mday,
                 cur_time_tm.tm_hour, cur_time_tm.tm_min, cur_time_tm.tm_sec,
                 diff_dump_time);

    std::map<std::pair<std::string, int>, StatInfo> stat_list_tmp;
    {
        MutexLock mlock(&mu_);
        stat_list_tmp = stat_list_;
        stat_list_.clear();
    }

    FormatWrite(fp, stat_list_tmp);

    fflush(fp);
    fclose(fp);

    return kOk;
}/*}}}*/

void Statistic::Print()
{/*{{{*/
    std::map<std::pair<std::string, int>, StatInfo> stat_list_tmp;
    {
        MutexLock mlock(&mu_);
        stat_list_tmp = stat_list_;
    }

    FormatWrite(stderr, stat_list_tmp);
}/*}}}*/

Code Statistic::FormatWrite(FILE *fp, const std::map<std::pair<std::string, int>, StatInfo> &stat_list)
{/*{{{*/
    if (fp == NULL) return kInvalidParam; 

    fprintf(fp, "|%8s|%8s|%8s|%10s|%10s|%10s|>=%4d(ms)|>=%4d(ms)|>=%4d(ms)|%8s|%8s|\n",
            "MODEL", "RESULT", "TOTAL", "MAX(ms)", "MIN(ms)", "AVG(ms)",
            time_out_level_one_, time_out_level_two_, time_out_level_three_, "RECV", "SEND");

    std::map<std::pair<std::string, int>, StatInfo>::const_iterator it = stat_list.begin();
    for (; it != stat_list.end(); ++it)
    {
        if (it->second.total_count <= 0) continue;

        fprintf(fp, "|%8s|%8d|%8d|%10.3f|%10.3f|%10.3f|%10d|%10d|%10d|%8lld|%8lld|\n",
                it->second.model.c_str(), it->second.ret_code, it->second.total_count,
                it->second.max_time/(float)1000, it->second.min_time/(float)1000,
                it->second.total_time/(float)1000/it->second.total_count,
                it->second.time_out_level_one_num, it->second.time_out_level_two_num,
                it->second.time_out_level_three_num, (long long int)it->second.total_recv_flow,
                (long long int)it->second.total_send_flow);
    }

    return kOk;
}/*}}}*/

Code Statistic::CheckIfNeedTransfer(bool *is_need_transfer)
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

Code Statistic::TransferFile()
{/*{{{*/
    std::string backup_path = path_ + kBackupSuffix;
    int ret = rename(path_.c_str(), backup_path.c_str());
    if (ret == -1) return kRenameFailed;

    return kOk;
}/*}}}*/

}

#ifdef _STATISTIC_MAIN_TEST_
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    using namespace base;

    std::string stat_path = "../demo/log/csutil.stat";
    int max_file_size = 1024;
    Statistic stat(stat_path, max_file_size);
    struct timeval start;
    gettimeofday(&start, NULL);
    srand(time(NULL));

    for (int i = 0; i < 1000; ++i)
    {
        struct timeval end;
        end.tv_sec = start.tv_sec+i;
        end.tv_usec = rand() % 1000000;
        stat.AddStat("Insert", i%3, start, end, i*10, i*20, 1);

//        stat.Print();
        if ((i+1)%10 == 0) stat.DumpStat();
        sleep(1);
    }

    stat.Print();

    return 0;
}
#endif
