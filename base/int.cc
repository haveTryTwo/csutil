// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "base/common.h"
#include "base/int.h"
#include "base/util.h"

namespace base {

static Code BigAddIntelnal(const std::string &post_ln, const std::string &post_rn,
                           std::string *sum);
static Code BigSubIntelnal(const std::string &post_ln, const std::string &post_rn,
                           std::string *result);
static Code BigMultiplyInternal(const std::string &post_ln, const std::string &post_rn,
                                std::string *result);

Code GetInt32(const std::string &str, int *num) { /*{{{*/
  int64_t num_tmp = 0;
  Code ret = GetInt64(str, &num_tmp);
  if (ret != kOk) return ret;

  *num = num_tmp;
  return kOk;
} /*}}}*/

Code GetUInt32(const std::string &str, uint32_t *num) { /*{{{*/
  uint64_t num_tmp = 0;
  Code ret = GetUInt64(str, &num_tmp);
  if (ret != kOk) return ret;

  *num = num_tmp;
  return kOk;
} /*}}}*/

Code GetInt64(const std::string &str, int64_t *num) { /*{{{*/
  return GetInt64(str, num, 10);
} /*}}}*/

Code GetUInt64(const std::string &str, uint64_t *num) { /*{{{*/
  return GetUInt64(str, num, 10);
} /*}}}*/

Code GetInt32ByHex(const std::string &hex_str, int *num) { /*{{{*/
  int64_t num_tmp = 0;
  Code ret = GetInt64ByHex(hex_str, &num_tmp);
  if (ret != kOk) return ret;

  *num = num_tmp;
  return kOk;
} /*}}}*/

Code GetUInt32ByHex(const std::string &hex_str, uint32_t *num) { /*{{{*/
  uint64_t num_tmp = 0;
  Code ret = GetUInt64ByHex(hex_str, &num_tmp);
  if (ret != kOk) return ret;

  *num = num_tmp;
  return kOk;
} /*}}}*/

Code GetInt64ByHex(const std::string &hex_str, int64_t *num) { /*{{{*/
  return GetInt64(hex_str, num, 16);
} /*}}}*/

Code GetUInt64ByHex(const std::string &hex_str, uint64_t *num) { /*{{{*/
  return GetUInt64(hex_str, num, 16);
} /*}}}*/

Code GetInt64(const std::string &str, int64_t *num, int base) { /*{{{*/
  if (num == NULL) return kInvalidParam;
  if (base < 0 || base > 36) return kInvalidParam;

  errno = 0;
  char *end_ptr = NULL;
  unsigned long long v = strtoull(str.c_str(), &end_ptr, base);
  if ((errno == ERANGE && (v == ULLONG_MAX)) || (errno != 0 && v == 0)) return kStrtollFailed;
  if (end_ptr == str.c_str()) return kNoDigits;
  if (*end_ptr != '\0') return kNotAllDigits;

  *num = (int64_t)v;
  return kOk;
} /*}}}*/

Code GetUInt64(const std::string &str, uint64_t *num, int base) { /*{{{*/
  if (num == NULL) return kInvalidParam;
  if (base < 0 || base > 36) return kInvalidParam;

  errno = 0;
  char *end_ptr = NULL;
  unsigned long long v = strtoull(str.c_str(), &end_ptr, base);
  if ((errno == ERANGE && (v == ULLONG_MAX)) || (errno != 0 && v == 0)) return kStrtollFailed;
  if (end_ptr == str.c_str()) return kNoDigits;
  if (*end_ptr != '\0') return kNotAllDigits;

  *num = v;
  return kOk;
} /*}}}*/

Code GetUpDivValue(uint64_t dividend, uint64_t divisor, uint64_t *value) { /*{{{*/
  if (divisor == 0 || value == NULL) return kInvalidParam;
  *value = (dividend + divisor - 1) / divisor;

  return kOk;
} /*}}}*/

Code BigAdd(const std::string &ln, const std::string &rn, std::string *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  std::string post_ln;
  std::string post_rn;
  bool is_ln_num = false;
  bool is_rn_num = false;
  bool is_ln_negative = false;
  bool is_rn_negative = false;

  Code ret = CheckAndGetIfIsAllNum(ln, &is_ln_num, &post_ln, &is_ln_negative);
  if (ret != kOk) return ret;
  if (!is_ln_num) return kNotAllDigits;

  ret = CheckAndGetIfIsAllNum(rn, &is_rn_num, &post_rn, &is_rn_negative);
  if (ret != kOk) return ret;
  if (!is_rn_num) return kNotAllDigits;

  if (is_ln_negative && is_rn_negative) {
    ret = BigAddIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;

    *result = "-" + *result;
  } else if (!is_ln_negative && !is_rn_negative) {
    ret = BigAddIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;
  } else if (is_ln_negative && !is_rn_negative) {
    ret = BigSubIntelnal(post_rn, post_ln, result);
    if (ret != kOk) return ret;
  } else if (!is_ln_negative && is_rn_negative) {
    ret = BigSubIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;
  }

  return kOk;
} /*}}}*/

Code BigSub(const std::string &ln, const std::string &rn, std::string *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  std::string post_ln;
  std::string post_rn;
  bool is_ln_num = false;
  bool is_rn_num = false;
  bool is_ln_negative = false;
  bool is_rn_negative = false;

  Code ret = CheckAndGetIfIsAllNum(ln, &is_ln_num, &post_ln, &is_ln_negative);
  if (ret != kOk) return ret;
  if (!is_ln_num) return kNotAllDigits;

  ret = CheckAndGetIfIsAllNum(rn, &is_rn_num, &post_rn, &is_rn_negative);
  if (ret != kOk) return ret;
  if (!is_rn_num) return kNotAllDigits;

  if (is_ln_negative && is_rn_negative) {
    ret = BigSubIntelnal(post_rn, post_ln, result);
    if (ret != kOk) return ret;
  } else if (!is_ln_negative && !is_rn_negative) {
    ret = BigSubIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;
  } else if (is_ln_negative && !is_rn_negative) {
    ret = BigAddIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;

    *result = "-" + *result;
  } else if (!is_ln_negative && is_rn_negative) {
    ret = BigAddIntelnal(post_ln, post_rn, result);
    if (ret != kOk) return ret;
  }

  return kOk;
} /*}}}*/

Code BigMultiply(const std::string &ln, const std::string &rn, std::string *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  std::string post_ln;
  std::string post_rn;
  bool is_ln_num = false;
  bool is_rn_num = false;
  bool is_ln_negative = false;
  bool is_rn_negative = false;

  Code ret = CheckAndGetIfIsAllNum(ln, &is_ln_num, &post_ln, &is_ln_negative);
  if (ret != kOk) return ret;
  if (!is_ln_num) return kNotAllDigits;

  ret = CheckAndGetIfIsAllNum(rn, &is_rn_num, &post_rn, &is_rn_negative);
  if (ret != kOk) return ret;
  if (!is_rn_num) return kNotAllDigits;

  ret = BigMultiplyInternal(post_ln, post_rn, result);
  if (ret != kOk) return ret;

  if (is_ln_negative && is_rn_negative) {
  } else if (!is_ln_negative && !is_rn_negative) {
  } else if (is_ln_negative && !is_rn_negative) {
    if (*result != "0") {
      *result = "-" + *result;
    }
  } else if (!is_ln_negative && is_rn_negative) {
    if (*result != "0") {
      *result = "-" + *result;
    }
  }

  return kOk;
} /*}}}*/

/**
 * Note: param post_ln and post_rn must be post number, and must't start with 0
 */
Code BigAddIntelnal(const std::string &post_ln, const std::string &post_rn,
                    std::string *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  uint32_t i = 0;
  std::string tmp_result;
  uint8_t carry_digit = 0;
  uint32_t post_ln_len = post_ln.size();
  uint32_t post_rn_len = post_rn.size();
  uint32_t min_len = post_ln_len < post_rn_len ? post_ln_len : post_rn_len;
  for (; i < min_len; ++i) { /*{{{*/
    uint8_t post_ln_digit = post_ln[post_ln.size() - 1 - i] - '0';
    uint8_t post_rn_digit = post_rn[post_rn.size() - 1 - i] - '0';
    uint8_t sum_digit = post_ln_digit + post_rn_digit + carry_digit;
    if (sum_digit >= 10) {
      carry_digit = 1;
      sum_digit -= 10;
      tmp_result.append(1, sum_digit + '0');
    } else {
      carry_digit = 0;
      tmp_result.append(1, sum_digit + '0');
    }
  } /*}}}*/

  for (; i < post_ln_len; ++i) { /*{{{*/
    uint8_t post_ln_digit = post_ln[post_ln.size() - 1 - i] - '0';
    uint8_t sum_digit = post_ln_digit + carry_digit;
    if (sum_digit >= 10) {
      carry_digit = 1;
      sum_digit -= 10;
      tmp_result.append(1, sum_digit + '0');
    } else {
      carry_digit = 0;
      tmp_result.append(1, sum_digit + '0');
    }
  } /*}}}*/

  for (; i < post_rn_len; ++i) { /*{{{*/
    uint8_t post_rn_digit = post_rn[post_rn.size() - 1 - i] - '0';
    uint8_t sum_digit = post_rn_digit + carry_digit;
    if (sum_digit >= 10) {
      carry_digit = 1;
      sum_digit -= 10;
      tmp_result.append(1, sum_digit + '0');
    } else {
      carry_digit = 0;
      tmp_result.append(1, sum_digit + '0');
    }
  } /*}}}*/

  if (carry_digit >= 1) {
    tmp_result.append(1, carry_digit + '0');
    carry_digit = 0;
  }

  Code ret = Reverse(tmp_result, result);
  if (ret != kOk) return ret;

  return kOk;
} /*}}}*/

/**
 * Note: param post_ln and post_rn must be post number, and must't start with 0
 */
Code BigSubIntelnal(const std::string &post_ln, const std::string &post_rn,
                    std::string *result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  int sub_flags = 0;
  std::string post_big;
  std::string post_litte;

  Code ret = GetBigAndLitteNum(post_ln, post_rn, &post_big, &post_litte, &sub_flags);
  if (ret != kOk) return ret;

  if (sub_flags == 0)  // It's euqal of two numbers
  {
    result->assign(1, kZero);
    return kOk;
  }

  uint32_t i = 0;
  std::string tmp_diff;
  int8_t borrow_digit = 0;
  uint32_t post_big_len = post_big.size();
  uint32_t post_litte_len = post_litte.size();

  for (i = 0; i < post_litte_len; ++i) {
    int8_t ln_digit = post_big[post_big.size() - 1 - i] - '0';
    int8_t rn_digit = post_litte[post_litte.size() - 1 - i] - '0';
    int difference = ln_digit - rn_digit + borrow_digit;
    if (difference < 0) {
      borrow_digit = -1;
      difference += 10;
    } else {
      borrow_digit = 0;
    }
    tmp_diff.append(1, difference + '0');
  }

  for (; i < post_big_len; ++i) {
    int8_t ln_digit = post_big[post_big.size() - 1 - i] - '0';
    int difference = ln_digit + borrow_digit;
    if (difference < 0) {
      borrow_digit = -1;
      difference += 10;
    } else {
      borrow_digit = 0;
    }
    tmp_diff.append(1, difference + '0');
  }

  if (borrow_digit != 0)  // Note: post_big > post_litte
  {
    return kSubError;
  }

  std::string tmp_result;
  ret = TrimRight(tmp_diff, kZero, &tmp_result);
  if (ret != kOk) return ret;

  if (sub_flags < 0) {
    tmp_result.append(1, '-');
  }

  ret = Reverse(tmp_result, result);
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

/**
 * Note: param post_ln and post_rn must be post number, and must't start with 0
 */
Code BigMultiplyInternal(const std::string &post_ln, const std::string &post_rn,
                         std::string *result) { /*{{{*/
  if (post_ln.empty() || post_rn.empty() || result == NULL) return kInvalidParam;

  uint32_t post_ln_len = post_ln.size();
  uint32_t post_rn_len = post_rn.size();
  std::vector<std::string> tmp_results;
  Code ret = kOk;

  for (uint32_t i = 0; i < post_ln_len; ++i) {
    uint8_t carry_digit = 0;
    std::string tmp_one_result(i, kZero);
    for (uint32_t j = 0; j < post_rn_len; ++j) {
      uint8_t post_ln_digit = post_ln[post_ln.size() - 1 - i] - '0';
      uint8_t post_rn_digit = post_rn[post_rn.size() - 1 - j] - '0';
      uint8_t sum_digit = post_ln_digit * post_rn_digit + carry_digit;
      carry_digit = sum_digit / kTen;
      sum_digit %= kTen;
      tmp_one_result.append(1, sum_digit + '0');
    }
    if (carry_digit > 0) {
      tmp_one_result.append(1, carry_digit + '0');
    }
    tmp_results.push_back(tmp_one_result);
  }

  for (uint32_t i = 0; i < tmp_results.size(); ++i) {
    std::string tmp_one_result;
    ret = Reverse(tmp_results[i], &tmp_one_result);
    if (ret != kOk) return ret;
    tmp_results[i] = tmp_one_result;
  }

  if (tmp_results.size() < 1) return kInvalidParam;

  std::string tmp_sum = tmp_results[0];
  for (uint32_t i = 1; i < tmp_results.size(); ++i) {
    std::string sum_result;
    ret = BigAddIntelnal(tmp_sum, tmp_results[i], &sum_result);
    if (ret != kOk) return ret;

    tmp_sum = sum_result;
  }

  ret = TrimLeft(tmp_sum, kZero, result);
  if (ret != kOk) return ret;

  if (result->empty()) {
    result->assign(1, kZero);
  }

  return ret;
} /*}}}*/

Code GCD(uint64_t first, uint64_t second, uint64_t *comm_divisor) { /*{{{*/
  if (comm_divisor == NULL) return kInvalidParam;

  if (first == 0 || second == 0 || first == 1 || second == 1) {
    *comm_divisor = 1;
    return kOk;
  }
  uint64_t tmp_first = first;
  uint64_t tmp_second = second;
  uint64_t divisor = tmp_first % tmp_second;
  while (divisor != 0) {
    tmp_first = tmp_second;
    tmp_second = divisor;
    divisor = tmp_first % tmp_second;
  }
  *comm_divisor = tmp_second;

  return kOk;
} /*}}}*/

Code LCM(uint32_t first, uint32_t second, uint64_t *comm_mul) { /*{{{*/
  if (comm_mul == NULL) return kInvalidParam;

  *comm_mul = 0;
  if (first == 0 || second == 0) {
    *comm_mul = 0;
    return kOk;
  }

  uint64_t max_comm_divisor = 1;
  Code ret = GCD(first, second, &max_comm_divisor);
  if (ret != kOk) return ret;

  *comm_mul = first * second / max_comm_divisor;

  return kOk;
} /*}}}*/

Code ReverseBits(uint64_t source, uint64_t *dest) { /*{{{*/
  if (dest == NULL) return kInvalidParam;

  uint32_t loop = 8 * sizeof(source);
  uint64_t mask = ~0;
  uint64_t tmp = source;
  for (loop >>= 1; loop > 0; loop >>= 1) {
    mask ^= (mask >> loop);
    tmp = (((tmp << loop) & mask) | ((tmp >> loop) & (~mask)));
  }
  *dest = tmp;
  return kOk;
} /*}}}*/

Code MaxContinuousSum(int64_t *arr, int len, int64_t *max_continuous_sum) { /*{{{*/
  if (arr == NULL || max_continuous_sum == NULL) return kInvalidParam;

  *max_continuous_sum = LLONG_MIN;
  int64_t tmp_part_sum = 0;
  for (int i = 0; i < len; ++i) {
    tmp_part_sum = tmp_part_sum + arr[i];

    if (tmp_part_sum > *max_continuous_sum) *max_continuous_sum = tmp_part_sum;

    if (tmp_part_sum < 0) {
      tmp_part_sum = 0;
    }
  }

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _INT_MAIN_TEST_
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;

  std::string str_dec = "11";
  std::string str_hex = "a0";
  int32_t num_32 = 0;
  int64_t num_64 = 0;

  Code ret = GetInt32(str_dec, &num_32);
  assert(ret == kOk);
  fprintf(stderr, "num_32:%d\n", num_32);

  ret = GetInt64(str_dec, &num_64);
  assert(ret == kOk);
  fprintf(stderr, "num_64:%lld\n", num_64);

  ret = GetInt32ByHex(str_hex, &num_32);
  assert(ret == kOk);
  fprintf(stderr, "num_32:%d\n", num_32);

  ret = GetInt64ByHex(str_hex, &num_64);
  assert(ret == kOk);
  fprintf(stderr, "num_64:%lld\n", num_64);

  str_dec = "1000050001341602082573830861";
  ret = GetInt64(str_dec, &num_64);
  if (ret != kOk) {
    fprintf(stderr, "Failed to get int64 of %s, ret:%d\n", str_dec.c_str(), ret);
  } else {
    fprintf(stderr, "num_64:%lld\n", num_64);
  }

  // GetUpDivValue Test
  uint64_t divisor = 4;
  for (int i = 0; i < 2 * divisor; ++i) {
    uint64_t dividend = 9 + i;
    uint64_t value = 0;
    ret = GetUpDivValue(dividend, divisor, &value);
    assert(ret == kOk);
    fprintf(stderr, "Up value of %llu / %llu = %llu\n", dividend, divisor, value);
  }

  return 0;
} /*}}}*/
#endif
