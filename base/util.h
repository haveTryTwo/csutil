// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_UTIL_H_
#define BASE_UTIL_H_

#include <fcntl.h>
#include <stdint.h>
#include <sys/socket.h>

#include <deque>
#include <set>
#include <string>

#include "base/common.h"
#include "base/status.h"

namespace base {

inline Code SetFdNonblock(int fd) { /*{{{*/
  int flags = fcntl(fd, F_GETFD);
  int ret = fcntl(fd, F_SETFD, flags | O_NONBLOCK);
  if (ret == -1) return kFcntlFailed;

  return kOk;
} /*}}}*/

inline Code SetFdReused(int fd) { /*{{{*/
  int opt = 1;
  int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
  if (ret == -1) return kSetsockoptFailed;
  return kOk;
} /*}}}*/

Code Trim(const std::string &in_cnt, char delim, std::string *out_cnt);
Code Trim(const std::string &in_cnt, const std::string &delims, std::string *out_cnt);
Code TrimLeft(const std::string &in_cnt, char delim, std::string *out_cnt);
Code TrimLeft(const std::string &in_cnt, const std::string &delims, std::string *out_cnt);
Code TrimRight(const std::string &in_cnt, char delim, std::string *out_cnt);
Code TrimRight(const std::string &in_cnt, const std::string &delims, std::string *out_cnt);

Code PrintBinStr(const std::string &str);

Code GetBinChar(uint8_t src_ch, uint8_t *dst_ch);
Code GetReadableStr(const std::string &bin_str, std::string *readable_str);
Code GetBinStr(const std::string &readable_str, std::string *bin_str);

Code Separate(const std::string &in_cnt, const std::string &delims, std::string *left_cnt, std::string *right_cnt);

Code Strtok(const std::string &cnt, char delim, std::deque<std::string> *words);
Code Strtok(const std::string &cnt, char delim, std::set<std::string> *words);

Code ToUpper(const std::string &src, std::string *dst);

Code CheckIsLowerCaseLetter(const std::string &str, bool *is_lower);

Code GetNumOfElements(const std::string &src, int delim, int *total_elemnts);

// element index starts with 1
Code GetElementOfIndex(const std::string &src, int element_index, int delim, std::string *element);

Code GetAndSetMaxFileNo();

Code CheckIsCplusplusFile(const std::string &path, bool *is_satisfied);

Code CheckAndGetIfIsAllNum(const std::string &num, bool *is_all_num, std::string *post_num, bool *is_negative = NULL);
Code CheckIsAllNum(const std::string &num, bool *is_all_num, bool *is_negative = NULL);
Code GetBigAndLitteNum(const std::string &post_ln, const std::string &post_rn, std::string *post_big,
                       std::string *post_litte, int *sub_flags);

Code Reverse(const std::string &src, std::string *dst);
Code Reverse(char *str, int len);
Code LoopShift(char *str, int len, int shift_num, ShiftType shift_type = kLeftShift);

Code GetOutputSuffix(uint64_t num, float *out_num, std::string *suffix);

bool CheckIsSkipChar(char ch);
bool CheckIsHyphen(char ch);

}  // namespace base

#endif
