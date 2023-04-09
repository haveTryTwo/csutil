// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "base/common.h"
#include "base/log.h"
#include "base/util.h"

namespace base {

Code Trim(const std::string &in_cnt, char delim, std::string *out_cnt) { /*{{{*/
  std::string buf;
  Code ret = TrimLeft(in_cnt, delim, &buf);
  if (ret != kOk) return ret;

  ret = TrimRight(buf, delim, out_cnt);
  return ret;
} /*}}}*/

Code Trim(const std::string &in_cnt, const std::string &delims, std::string *out_cnt) { /*{{{*/
  std::string buf;
  Code ret = TrimLeft(in_cnt, delims, &buf);
  if (ret != kOk) return ret;

  ret = TrimRight(buf, delims, out_cnt);
  return ret;
} /*}}}*/

Code TrimLeft(const std::string &in_cnt, char delim, std::string *out_cnt) { /*{{{*/
  if (out_cnt == NULL) return kInvalidParam;

  int i = 0;
  for (; i < (int)in_cnt.size(); ++i) {
    if (in_cnt.data()[i] != delim) break;
  }

  out_cnt->assign(in_cnt.data() + i, in_cnt.size() - i);
  return kOk;
} /*}}}*/

Code TrimLeft(const std::string &in_cnt, const std::string &delims, std::string *out_cnt) { /*{{{*/
  if (out_cnt == NULL) return kInvalidParam;

  int i = 0;
  for (; i < (int)in_cnt.size(); ++i) {
    if (delims.find(in_cnt.data()[i]) == std::string::npos) break;
  }

  out_cnt->assign(in_cnt.data() + i, in_cnt.size() - i);
  return kOk;
} /*}}}*/

Code TrimRight(const std::string &in_cnt, char delim, std::string *out_cnt) { /*{{{*/
  if (out_cnt == NULL) return kInvalidParam;

  int i = in_cnt.size() - 1;
  for (; i >= 0; --i) {
    if (in_cnt.data()[i] != delim) break;
  }

  out_cnt->assign(in_cnt.data(), i + 1);
  return kOk;
} /*}}}*/

Code TrimRight(const std::string &in_cnt, const std::string &delims, std::string *out_cnt) { /*{{{*/
  if (out_cnt == NULL) return kInvalidParam;

  int i = in_cnt.size() - 1;
  for (; i >= 0; --i) {
    if (delims.find(in_cnt.data()[i]) == std::string::npos) break;
  }

  out_cnt->assign(in_cnt.data(), i + 1);
  return kOk;
} /*}}}*/

Code PrintBinStr(const std::string &str) { /*{{{*/
  for (size_t i = 0; i < str.size(); ++i) {
    fprintf(stderr, "%02x", (uint8_t)str.data()[i]);
  }
  fprintf(stderr, "\n");

  return kOk;
} /*}}}*/

Code GetBinChar(uint8_t src_ch, uint8_t *dst_ch) { /*{{{*/
  if (dst_ch == NULL) return kInvalidParam;

  if (src_ch >= '0' && src_ch <= '9') {
    *dst_ch = src_ch - '0';
  } else if (src_ch >= 'a' && src_ch <= 'f') {
    *dst_ch = src_ch - 'a' + 0xa;
  } else {
    return kInvalidParam;
  }

  return kOk;
} /*}}}*/

Code GetReadableStr(const std::string &bin_str, std::string *readable_str) { /*{{{*/
  if (readable_str == NULL) return kInvalidParam;

  readable_str->clear();
  for (size_t i = 0; i < bin_str.size(); ++i) {
    char buf[3] = {0};
    snprintf(buf, sizeof(buf), "%02x", (uint8_t)bin_str.data()[i]);
    readable_str->append(buf);
  }

  return kOk;
} /*}}}*/

Code GetBinStr(const std::string &readable_str, std::string *bin_str) { /*{{{*/
  if (bin_str == NULL) return kInvalidParam;
  if (readable_str.size() % 2 != 0) return kInvalidParam;

  Code ret = kOk;
  bin_str->clear();
  for (size_t i = 0; i < readable_str.size(); i += 2) {
    uint8_t tmp_ch = 0;
    ret = GetBinChar(readable_str[i], &tmp_ch);
    if (ret != kOk) return ret;
    uint8_t num = tmp_ch << 4;

    ret = GetBinChar(readable_str[i + 1], &tmp_ch);
    if (ret != kOk) return ret;
    num += tmp_ch;

    bin_str->append(1, num);
  }

  return ret;
} /*}}}*/

Code Separate(const std::string &in_cnt, const std::string &delims, std::string *left_cnt,
              std::string *right_cnt) { /*{{{*/
  std::size_t pos = in_cnt.find(delims);
  if (pos == std::string::npos) return kNotFound;

  left_cnt->assign(in_cnt.data(), pos);
  right_cnt->assign(in_cnt.data() + pos + delims.size(), in_cnt.size() - pos - delims.size());
  return kOk;
} /*}}}*/

Code Strtok(const std::string &cnt, char delim, std::deque<std::string> *words) { /*{{{*/
  if (words == NULL) return kInvalidParam;

  Code ret = kOk;
  const char *start = cnt.c_str();

  while (true) {
    const char *pos = strchr(start, delim);
    if (pos == NULL) {
      words->push_back(start);
      break;
    }

    words->push_back(std::string(start, pos - start));
    start = pos + 1;
  }

  return ret;
} /*}}}*/

Code ToUpper(const std::string &src, std::string *dst) { /*{{{*/
  if (dst == NULL) return kInvalidParam;

  dst->clear();
  for (int i = 0; i < (int)src.size(); ++i) {
    if (src.data()[i] >= 'a' && src.data()[i] <= 'z')
      dst->append(1, src.data()[i] - 32);
    else
      dst->append(src.data() + i, 1);
  }

  return kOk;
} /*}}}*/

Code CheckIsLowerCaseLetter(const std::string &str, bool *is_lower) { /*{{{*/
  if (is_lower == NULL) return kInvalidParam;
  *is_lower = false;

  for (uint32_t i = 0; i < str.size(); ++i) {
    if (str.data()[i] < 'a' || str.data()[i] > 'z') {
      return kOk;
    }
  }

  *is_lower = true;
  return kOk;
} /*}}}*/

Code GetNumOfElements(const std::string &src, int delim, int *total_elemnts) { /*{{{*/
  if (total_elemnts == NULL) return kInvalidParam;

  int num = 0;
  const char *cnt = src.c_str();

  while (*cnt != '\0') {
    const char *pos = strchr(cnt, delim);
    if (pos == NULL) break;

    cnt = pos + 1;
    num++;
  }

  // the number of elements is number of delim plus one
  *total_elemnts = num + 1;

  return kOk;
} /*}}}*/

Code GetElementOfIndex(const std::string &src, int element_index, int delim,
                       std::string *element) { /*{{{*/
  if (element_index <= 0 || element == NULL) return kInvalidParam;

  int num = 0;
  const char *cnt = src.c_str();
  while (*cnt != '\0') {
    const char *pos = strchr(cnt, delim);
    if (pos == NULL) break;

    if (num == element_index - 1) {
      element->assign(cnt, pos - cnt);
      return kOk;
    }

    cnt = pos + 1;
    num++;
  }

  if (num == element_index - 1) {
    element->assign(cnt, src.data() + src.size() - cnt);
    return kOk;
  }

  return kNotFound;
} /*}}}*/

Code GetAndSetMaxFileNo() { /*{{{*/
  Code ret = kOk;

  struct rlimit rlim = {0};
  int r = getrlimit(RLIMIT_NOFILE, &rlim);
  if (r == -1) {
    LOG_ERR("Failed to get rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
    return kGetRlimitFailed;
  }
  LOG_INFO("before set! cur number of file descriptors:%d, max number of file descriptors:%d",
           (int)(rlim.rlim_cur), (int)(rlim.rlim_max));

  if (rlim.rlim_cur < rlim.rlim_max) {
    rlim.rlim_cur = rlim.rlim_max;

    r = setrlimit(RLIMIT_NOFILE, &rlim);
    if (r == -1) {
      LOG_ERR("Failed to set rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
      return kSetRlimitFailed;
    }
  }

  r = getrlimit(RLIMIT_NOFILE, &rlim);
  if (r == -1) {
    LOG_ERR("Failed to get rlimit of fileno! errno:%d, err:%s", errno, strerror(errno));
    return kGetRlimitFailed;
  }
  LOG_INFO("after set! cur number of file descriptors:%d, max number of file descriptors:%d",
           (int)(rlim.rlim_cur), (int)(rlim.rlim_max));

  return ret;
} /*}}}*/

Code CheckIsCplusplusFile(const std::string &path, bool *is_satisfied) { /*{{{*/
  std::vector<std::string> cc_suffix;
  cc_suffix.push_back(".h");
  cc_suffix.push_back(".cc");
  cc_suffix.push_back(".cpp");

  Code ret = kOk;
  *is_satisfied = false;
  std::vector<std::string>::iterator vec_it;
  for (vec_it = cc_suffix.begin(); vec_it != cc_suffix.end(); ++vec_it) {
    size_t pos = path.find(*vec_it);
    if (pos != std::string::npos && (pos + vec_it->size() == path.size())) {
      *is_satisfied = true;
      break;
    }
  }

  return ret;
} /*}}}*/

Code CheckAndGetIfIsAllNum(const std::string &num, bool *is_all_num, std::string *post_num,
                           bool *is_negative) { /*{{{*/
  if (is_all_num == NULL || post_num == NULL) return kInvalidParam;

  *is_all_num = false;
  post_num->clear();
  if (is_negative != NULL) *is_negative = false;

  bool has_num = false;
  std::string tmp_num;
  std::string delims(" +\t\r\n");
  Code ret = Trim(num, delims, &tmp_num);
  if (ret != kOk) return ret;

  for (int i = 0; i < (int)tmp_num.size(); ++i) { /*{{{*/
    if (tmp_num.data()[i] <= '9' && tmp_num.data()[i] >= '0') {
      has_num = true;
    } else if (i == 0 && tmp_num.data()[i] == '-') {
      continue;
    } else {
      return kOk;
    }
  } /*}}}*/

  if (has_num) { /*{{{*/
    *is_all_num = true;
    if (tmp_num.size() > 0 && tmp_num.data()[0] == '-') {
      if (is_negative != NULL) {
        *is_negative = true;
      }

      ret = TrimLeft(tmp_num.substr(1), kZero, post_num);
      if (ret != kOk) return ret;
    } else {
      ret = TrimLeft(tmp_num, kZero, post_num);
      if (ret != kOk) return ret;
    }

    if (post_num->empty()) {
      post_num->assign(1, kZero);
    }
  } /*}}}*/

  return kOk;
} /*}}}*/

Code CheckIsAllNum(const std::string &num, bool *is_all_num, bool *is_negative) { /*{{{*/
  if (is_all_num == NULL) return kInvalidParam;

  std::string post_num;

  return CheckAndGetIfIsAllNum(num, is_all_num, &post_num, is_negative);
} /*}}}*/

Code GetBigAndLitteNum(const std::string &post_ln, const std::string &post_rn,
                       std::string *post_big, std::string *post_litte, int *sub_flags) { /*{{{*/
  if (post_big == NULL || post_litte == NULL || sub_flags == NULL) return kInvalidParam;

  uint32_t i = 0;
  *sub_flags = 0;
  uint32_t post_ln_len = post_ln.size();
  uint32_t post_rn_len = post_rn.size();

  if (post_ln_len > post_rn_len) {
    *post_big = post_ln;
    *post_litte = post_rn;
    *sub_flags = 1;
  } else if (post_ln_len < post_rn_len) {
    *post_big = post_rn;
    *post_litte = post_ln;
    *sub_flags = -1;
  } else { /*{{{*/
    for (i = 0; i < post_ln_len; ++i) {
      if (post_ln[i] > post_rn[i]) {
        *post_big = post_ln;
        *post_litte = post_rn;
        *sub_flags = 1;
        break;
      } else if (post_ln[i] < post_rn[i]) {
        *post_big = post_rn;
        *post_litte = post_ln;
        *sub_flags = -1;
        break;
      }
    }

    if (i == post_ln_len)  // It's equal of two numbers;
    {
      *sub_flags = 0;
      return kOk;
    }
  } /*}}}*/

  return kOk;
} /*}}}*/

Code Reverse(const std::string &src, std::string *dst) { /*{{{*/
  if (dst == NULL) return kInvalidParam;

  dst->clear();

  for (uint32_t i = 0; i < src.size(); ++i) {
    dst->append(1, src[src.size() - 1 - i]);
  }

  return kOk;
} /*}}}*/

Code Reverse(char *str, int len) { /*{{{*/
  if (str == NULL || len < 0) return kInvalidParam;

  char tmp = 0;
  for (int i = 0; i < len / 2; ++i) {
    tmp = str[i];
    str[i] = str[len - 1 - i];
    str[len - 1 - i] = tmp;
  }

  return kOk;
} /*}}}*/

Code LoopShift(char *str, int len, int shift_num, ShiftType shift_type) { /*{{{*/
  if (str == NULL || len < 0 || shift_num < 0 || len < shift_num) return kInvalidParam;
  if (!(shift_type == kLeftShift || shift_type == kRightShift)) return kInvalidParam;

  if ((len == 0) || (len == shift_num))
    return kOk;  // NOTE: if shift whole string, then just return for it's satisfied

  Code ret = kOtherFailed;
  switch (shift_type) {
    case kLeftShift:
      ret = Reverse(str, shift_num);
      if (ret != kOk) return ret;
      ret = Reverse(str + shift_num, len - shift_num);
      if (ret != kOk) return ret;
      ret = Reverse(str, len);
      if (ret != kOk) return ret;
      break;
    case kRightShift:
      ret = Reverse(str, len - shift_num);
      if (ret != kOk) return ret;
      ret = Reverse(str + len - shift_num, shift_num);
      if (ret != kOk) return ret;
      ret = Reverse(str, len);
      if (ret != kOk) return ret;
      break;
    default:
      return kInvalidParam;
  }

  return ret;
} /*}}}*/

Code GetOutputSuffix(uint64_t num, float *out_num, std::string *suffix) { /*{{{*/
  if (out_num == NULL || suffix == NULL) return kInvalidParam;

  double tmp_num = num;
  std::string suffix_arr[] = {"B", "KB", "MB", "GB", "TB"};
  for (uint8_t i = 0; i < (uint8_t)(sizeof(suffix_arr) / sizeof(suffix_arr[0])); ++i) {
    *suffix = suffix_arr[i];
    if (tmp_num < kKB) {
      *out_num = (float)tmp_num;
      return kOk;
    }

    tmp_num = tmp_num / kKB;
  }

  *suffix = "PB";
  *out_num = (float)tmp_num;
  return kOk;
} /*}}}*/

bool CheckIsSkipChar(char ch) { /*{{{*/
  return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
} /*}}}*/

bool CheckIsHyphen(char ch) { /*{{{*/
  return (ch == '_' || ch == '+' || ch == '-');
} /*}}}*/

}  // namespace base

#ifdef _UTIL_MAIN_TEST_
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string in_cnt("###zhang san###");
  std::string out_cnt;
  char delim = '#';

  Code ret = Trim(in_cnt, delim, &out_cnt);
  assert(ret == kOk);
  fprintf(stderr, "size:%lu, in_cnt:%s\n", in_cnt.size(), in_cnt.c_str());
  fprintf(stderr, "size:%lu, out_cnt:%s\n", out_cnt.size(), out_cnt.c_str());

  // Test Separete
  in_cnt.assign("name === wangwu");
  std::string left_cnt;
  std::string right_cnt;
  std::string delims = "===";
  ret = Separate(in_cnt, delims, &left_cnt, &right_cnt);
  assert(ret == kOk);
  fprintf(stderr, "in_cnt:%s, left:%s, right:%s\n", in_cnt.c_str(), left_cnt.c_str(),
          right_cnt.c_str());

  // Test strtok
  in_cnt.assign("aaa,bbb,cc,d");
  std::deque<std::string> columns;
  ret = Strtok(in_cnt, ',', &columns);
  assert(ret == kOk);
  std::deque<std::string>::iterator it;
  fprintf(stderr, "size of columns:%zu\n", columns.size());
  for (it = columns.begin(); it != columns.end(); ++it) fprintf(stderr, "%s\n", it->c_str());
  fprintf(stderr, "\n");

  columns.clear();
  in_cnt.assign(",a,,b,");
  ret = Strtok(in_cnt, ',', &columns);
  assert(ret == kOk);
  fprintf(stderr, "size of columns:%zu\n", columns.size());
  for (it = columns.begin(); it != columns.end(); ++it) fprintf(stderr, "%s\n", it->c_str());
  fprintf(stderr, "\n");

  in_cnt.assign("a,b,,c,,");
  int total_elemnts = 0;
  ret = GetNumOfElements(in_cnt, ',', &total_elemnts);
  assert(ret == kOk);
  fprintf(stderr, "number of elements:%s is:%d\n", in_cnt.c_str(), total_elemnts);

  for (int i = 0; i < total_elemnts + 1; ++i) {
    std::string element;
    ret = GetElementOfIndex(in_cnt, i + 1, ',', &element);
    if (ret != kOk) {
      fprintf(stderr, "Not found of index:%d, ret:%d\n", i + 1, ret);
    } else {
      fprintf(stderr, "index:%d of str:\"%s\" is \"%s\"\n", i + 1, in_cnt.c_str(), element.c_str());
    }
  }

  // Test TrimLeft
  in_cnt = "#! \t@LOG_NAME(";
  delims = "#!@ \t";
  ret = TrimLeft(in_cnt, delims, &out_cnt);
  assert(ret == kOk);
  fprintf(stderr, "size:%lu, in_cnt:%s\n", in_cnt.size(), in_cnt.c_str());
  fprintf(stderr, "size:%lu, out_cnt:%s\n", out_cnt.size(), out_cnt.c_str());

  std::string log_name = "LOG_NAME";
  if (out_cnt.compare(0, log_name.size(), log_name) == 0) {
    fprintf(stderr, "log_name:%s is in\n", log_name.c_str());
  } else {
    fprintf(stderr, "log_name:%s is not in\n", log_name.c_str());
  }

  in_cnt = "LOG_NAME( ); \t\n\r";
  delims = " \t\r\n";
  ret = TrimRight(in_cnt, delims, &out_cnt);
  assert(ret == kOk);
  fprintf(stderr, "size:%lu, in_cnt:%s\n", in_cnt.size(), in_cnt.c_str());
  fprintf(stderr, "size:%lu, out_cnt:%s\n", out_cnt.size(), out_cnt.c_str());

  bool is_satisfied = false;
  std::string path = "aa.py";
  ret = CheckIsCplusplusFile(path, &is_satisfied);
  assert(ret == kOk);
  if (is_satisfied) {
    fprintf(stderr, "[SATISFY] path:%s is cplusplus\n", path.c_str());
  } else {
    fprintf(stderr, "[NOT SATISFY] path:%s is not cplusplus\n", path.c_str());
  }

  return 0;
} /*}}}*/
#endif
