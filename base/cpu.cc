// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>

#include "base/cpu.h"

namespace base
{

#if defined(__linux__) || defined(__unix__)

CPU::CPU() : cpu_max_num_(0), cpu_bit_size_(0), cpu_sets_(NULL)
{/*{{{*/
}/*}}}*/

CPU::~CPU()
{/*{{{*/
    if (cpu_sets_ != NULL)
    {
        CPU_FREE(cpu_sets_);
        cpu_sets_ = NULL;
    }
}/*}}}*/

Code CPU::Init(int cpu_max_num)
{/*{{{*/
    cpu_sets_ = CPU_ALLOC(cpu_max_num);
    if (cpu_sets_ == NULL) return kCPUError;

    cpu_max_num_ = cpu_max_num;
    cpu_bit_size_ = CPU_ALLOC_SIZE(cpu_max_num);

    CPU_ZERO_S(cpu_bit_size_, cpu_sets_);

    return kOk;
}/*}}}*/

Code CPU::Add(int cpu_idx)
{/*{{{*/
    CPU_SET_S(cpu_idx, cpu_bit_size_, cpu_sets_);
    cpu_idxs_.insert(cpu_idx);
    int ret = sched_setaffinity(0, cpu_bit_size_, cpu_sets_);
    if (ret != 0) return kCPUError;

    return kOk;
}/*}}}*/

Code CPU::Del(int cpu_idx)
{/*{{{*/
    CPU_CLR_S(cpu_idx, cpu_bit_size_, cpu_sets_);
    cpu_idxs_.erase(cpu_idx);
    int ret = sched_setaffinity(0, cpu_bit_size_, cpu_sets_);
    if (ret != 0) return kCPUError;

    return kOk;
}/*}}}*/

Code CPU::Count(int *cpu_num) const
{/*{{{*/
    if (cpu_num == NULL) return kInvalidParam;

    *cpu_num = CPU_COUNT_S(cpu_bit_size_, cpu_sets_);

    return kOk;
}/*}}}*/

Code CPU::Isset(int cpu_idx, bool *is_exist) const
{/*{{{*/
    if (is_exist == NULL) return kInvalidParam;

    *is_exist = CPU_ISSET_S(cpu_idx, cpu_bit_size_, cpu_sets_);

    return kOk;
}/*}}}*/

Code CPU::GetCpuInfo(std::set<int> *cpu_info) const
{/*{{{*/
    if (cpu_info == NULL) return kInvalidParam;

    *cpu_info = cpu_idxs_;

    return kOk;
}/*}}}*/

Code CPU::Clear()
{/*{{{*/
    CPU_ZERO_S(cpu_bit_size_, cpu_sets_);
    cpu_idxs_.clear();

    return kOk;
}/*}}}*/

#endif

Code GetCPUNum(int *cpu_num)
{/*{{{*/
    if (cpu_num == NULL) return kInvalidParam;
    int num = sysconf(_SC_NPROCESSORS_ONLN);
    if (num < 0) return kCPUError;
    *cpu_num = num;
    return kOk;
}/*}}}*/

}

#ifdef _CPU_MAIN_TEST_
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

#if defined(__linux__) || defined(__unix__)
    CPU cpu;
    cpu.Init(100);
    cpu.Add(1);
    cpu.Add(3);
    std::set<int> set;
    typedef std::set<int>::iterator SetIter;
    cpu.GetCpuInfo(&set);
    for (SetIter it = set.begin(); it != set.end(); ++it)
    {
        fprintf(stderr, "%d  ", *it);
    }
    fprintf(stderr, "\n");

    for (int i = 0; i < 3000000; ++i)
    {
        for (int j = 0; j < 1000; ++j);
    }

    set.clear();
    cpu.Del(1);
    cpu.GetCpuInfo(&set);
    for (SetIter it = set.begin(); it != set.end(); ++it)
    {
        fprintf(stderr, "%d  ", *it);
    }
    fprintf(stderr, "\n");

    for (int i = 0; i < 3000000; ++i)
    {
        for (int j = 0; j < 1000; ++j);
    }
#endif

    return 0;
}/*}}}*/
#endif
