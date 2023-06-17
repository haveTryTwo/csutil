// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CPU_H_
#define BASE_CPU_H_

#include <set>

#include <sched.h>

#include "base/status.h"

namespace base {

#if defined(__linux__) || defined(__unix__)
class CPU { /*{{{*/
 public:
  CPU();
  ~CPU();

  Code Init(int cpu_max_num);

  Code Add(int cpu_idx);
  Code Del(int cpu_idx);
  Code Count(int *cpu_num) const;
  Code Isset(int cpu_idx, bool *is_exist) const;
  Code GetCpuInfo(std::set<int> *cpu_info) const;
  Code Clear();

 private:
  int cpu_max_num_;      // The max number of cpus
  size_t cpu_bit_size_;  // The size in bytes that would be needed
                         // to hold cpus in range of [0, cpu_max_num_]
  cpu_set_t *cpu_sets_;
  std::set<int> cpu_idxs_;
}; /*}}}*/
#endif

Code GetCPUNum(int *cpu_num);

}  // namespace base

#endif
