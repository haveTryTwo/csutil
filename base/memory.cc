// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "base/common.h"
#include "base/memory.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#endif

namespace base {

Code GetProcessMemoryUsage(uint64_t *memory_usage) { /*{{{*/
  if (memory_usage == NULL) return kInvalidParam;

#if defined(__linux__)
  std::ifstream statm("/proc/self/statm");
  long size = 0;
  long resident = 0;
  long share = 0;
  long text = 0;
  long lib = 0;
  long data = 0;
  long dt = 0;
  statm >> size >> resident >> share >> text >> lib >> data >> dt;
  statm.close();

  long page_size = sysconf(_SC_PAGESIZE);
  *memory_usage = resident * page_size;
#elif defined(__APPLE__) && defined(__MACH__)
  task_basic_info_data_t task_info_data;
  mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;

  if (task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&task_info_data, &info_count) !=
      KERN_SUCCESS) {
    return base::kTaskInfoFailed;
  }

  *memory_usage = task_info_data.resident_size;
#else
  return base::kNotSupportOS;
#endif

  return base::kOk;
} /*}}}*/

Code GetProcessMemoryReadableUsage(std::string *memory_usage_readable) { /*{{{*/
  if (memory_usage_readable == NULL) return kInvalidParam;

  uint64_t memory_usage = 0;
  base::Code ret = GetProcessMemoryUsage(&memory_usage);
  if (ret != base::kOk) return ret;

  char buf[kSmallBufLen] = {0};
  if (memory_usage >= kGB) {
    snprintf(buf, sizeof(buf) - 1, "%5.2fGB", 1.0 * memory_usage / kGB);
  } else if (memory_usage >= kMB) {
    snprintf(buf, sizeof(buf) - 1, "%5.2fMB", 1.0 * memory_usage / kMB);
  } else if (memory_usage >= kKB) {
    snprintf(buf, sizeof(buf) - 1, "%5.2fKB", 1.0 * memory_usage / kKB);
  } else {
    snprintf(buf, sizeof(buf) - 1, "%lluB", memory_usage);
  }

  memory_usage_readable->assign(buf);

  return base::kOk;
} /*}}}*/

}  // namespace base
