// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "base/int.h"
#include "base/util.h"

namespace base
{

Code GetInt32(const std::string &str, int *num)
{/*{{{*/
    int64_t num_tmp = 0;
    Code ret = GetInt64(str, &num_tmp);
    if (ret != kOk) return ret;

    *num = num_tmp;
    return kOk;
}/*}}}*/

Code GetUInt32(const std::string &str, uint32_t *num)
{/*{{{*/
    uint64_t num_tmp = 0;
    Code ret = GetUInt64(str, &num_tmp);
    if (ret != kOk) return ret;

    *num = num_tmp;
    return kOk;
}/*}}}*/

Code GetInt64(const std::string &str, int64_t *num)
{/*{{{*/
    return GetInt64(str, num, 10);
}/*}}}*/

Code GetUInt64(const std::string &str, uint64_t *num)
{/*{{{*/
    return GetUInt64(str, num, 10);
}/*}}}*/

Code GetInt32ByHex(const std::string &hex_str, int *num)
{/*{{{*/
    int64_t num_tmp = 0;
    Code ret = GetInt64ByHex(hex_str, &num_tmp);
    if (ret != kOk) return ret;

    *num = num_tmp;
    return kOk;
}/*}}}*/

Code GetUInt32ByHex(const std::string &hex_str, uint32_t *num)
{/*{{{*/
    uint64_t num_tmp = 0;
    Code ret = GetUInt64ByHex(hex_str, &num_tmp);
    if (ret != kOk) return ret;

    *num = num_tmp;
    return kOk;
}/*}}}*/

Code GetInt64ByHex(const std::string &hex_str, int64_t *num)
{/*{{{*/
    return GetInt64(hex_str, num, 16);
}/*}}}*/

Code GetUInt64ByHex(const std::string &hex_str, uint64_t *num)
{/*{{{*/
    return GetUInt64(hex_str, num, 16);
}/*}}}*/


Code GetInt64(const std::string &str, int64_t *num, int base)
{/*{{{*/
    if (num == NULL) return kInvalidParam;
    if (base < 0 || base > 36) return kInvalidParam;

    errno = 0;
    char *end_ptr = NULL;
    unsigned long long v = strtoull(str.c_str(), &end_ptr, base);
    if ((errno == ERANGE && (v == ULLONG_MAX)) ||
            (errno != 0 && v == 0))
        return kStrtollFailed;
    if (end_ptr == str.c_str()) return kNoDigits;
    if (*end_ptr != '\0') return kNotAllDigits;

    *num = (int64_t)v;
    return kOk;
}/*}}}*/

Code GetUInt64(const std::string &str, uint64_t *num, int base)
{/*{{{*/
    if (num == NULL) return kInvalidParam;
    if (base < 0 || base > 36) return kInvalidParam;

    errno = 0;
    char *end_ptr = NULL;
    unsigned long long v = strtoull(str.c_str(), &end_ptr, base);
    if ((errno == ERANGE && (v == ULLONG_MAX)) ||
            (errno != 0 && v == 0))
        return kStrtollFailed;
    if (end_ptr == str.c_str()) return kNoDigits;
    if (*end_ptr != '\0') return kNotAllDigits;

    *num = v;
    return kOk;
}/*}}}*/

Code GetUpDivValue(uint64_t dividend, uint64_t divisor, uint64_t *value)
{/*{{{*/
    if (divisor == 0 || value == NULL) return kInvalidParam;
    *value = (dividend + divisor - 1) / divisor;

    return kOk;
}/*}}}*/

Code BigAdd(const std::string &ln, const std::string &rn, std::string *result)
{/*{{{*/
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

    // Not support subtraction
    if (is_ln_negative || is_rn_negative) return kNotPostiveDigits;

    uint32_t i = 0;
    std::string tmp_sum;
    uint8_t carry_digit = 0;
    uint32_t post_ln_len = post_ln.size();
    uint32_t post_rn_len = post_rn.size();
    uint32_t min_len = post_ln_len < post_rn_len ? post_ln_len : post_rn_len;
    for (; i < min_len; ++i)
    {/*{{{*/
        uint8_t post_ln_digit = post_ln[post_ln.size()-1-i] - '0';
        uint8_t post_rn_digit = post_rn[post_rn.size()-1-i] - '0';
        uint8_t sum_digit = post_ln_digit + post_rn_digit + carry_digit;
        if (sum_digit >= 10)
        {
            carry_digit = 1;
            sum_digit -= 10;
            tmp_sum.append(1, sum_digit+'0');
        }
        else
        {
            carry_digit = 0;
            tmp_sum.append(1, sum_digit+'0');
        }
    }/*}}}*/

    for (; i < post_ln_len; ++i)
    {/*{{{*/
        uint8_t post_ln_digit = post_ln[post_ln.size()-1-i] - '0';
        uint8_t sum_digit = post_ln_digit + carry_digit;
        if (sum_digit >= 10)
        {
            carry_digit = 1;
            sum_digit -= 10;
            tmp_sum.append(1, sum_digit+'0');
        }
        else
        {
            carry_digit = 0;
            tmp_sum.append(1, sum_digit+'0');
        }
    }/*}}}*/

    for (; i < post_rn_len; ++i)
    {/*{{{*/
        uint8_t post_rn_digit = post_rn[post_rn.size()-1-i] - '0';
        uint8_t sum_digit = post_rn_digit + carry_digit;
        if (sum_digit >= 10)
        {
            carry_digit = 1;
            sum_digit -= 10;
            tmp_sum.append(1, sum_digit+'0');
        }
        else
        {
            carry_digit = 0;
            tmp_sum.append(1, sum_digit+'0');
        }
    }/*}}}*/

    if (carry_digit >= 1)
    {
        tmp_sum.append(1, carry_digit+'0');
        carry_digit = 0;
    }

    ret = Reverse(tmp_sum, result);
    if (ret != kOk) return ret;

    return kOk;
}/*}}}*/

Code GetMaxCommonDivisor(uint64_t first, uint64_t second, uint64_t *comm_divisor)
{/*{{{*/
    if (comm_divisor == NULL) return kInvalidParam;

    if (first == 0 || second == 0 || first == 1 || second == 1)
    {
        *comm_divisor = 1;
        return kOk;
    }

    uint64_t max_divisor = first < second ? first/2 : second/2;
    uint64_t tmp_max_comm_divisor = 1;

    for (uint64_t i = 1; i < max_divisor; ++i)
    {
        if ((first % i == 0) && (second % i == 0))
        {
            tmp_max_comm_divisor = tmp_max_comm_divisor > i ? tmp_max_comm_divisor : i;
        }
    }

    *comm_divisor = tmp_max_comm_divisor;

    return kOk;
}/*}}}*/

Code GetMinCommonMultiple(uint32_t first, uint32_t second, uint64_t *comm_mul)
{/*{{{*/
    if (comm_mul == NULL) return kInvalidParam;
    
    *comm_mul = 0;
    if (first == 0 || second == 0)
    {
        *comm_mul = 0;
        return kOk;
    }

    uint64_t max_comm_divisor = 1;
    Code ret = GetMaxCommonDivisor(first, second, &max_comm_divisor);
    if (ret != kOk) return ret;

    *comm_mul = first * second / max_comm_divisor;

    return kOk;
}/*}}}*/

Code ReverseBits(uint64_t source, uint64_t *dest)
{/*{{{*/
    if (dest == NULL) return kInvalidParam;

    uint32_t loop = 8 * sizeof(source);
    uint64_t mask = ~0;
    uint64_t tmp = source;
    for (loop >>= 1; loop > 0; loop >>= 1)
    {
        mask ^= (mask >> loop);
        tmp = (((tmp<<loop) & mask) | ((tmp>>loop)&(~mask)));
    }
    *dest = tmp;
    return kOk;
}/*}}}*/

}

#ifdef _INT_MAIN_TEST_
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{/*{{{*/
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
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to get int64 of %s, ret:%d\n", str_dec.c_str(), ret);
    }
    else
    {
        fprintf(stderr, "num_64:%lld\n", num_64);
    }

    // GetUpDivValue Test
    uint64_t divisor = 4;
    for (int i = 0; i < 2*divisor; ++i)
    {
        uint64_t dividend = 9+i;
        uint64_t value = 0;
        ret = GetUpDivValue(dividend, divisor, &value);
        assert(ret == kOk);
        fprintf(stderr, "Up value of %llu / %llu = %llu\n", dividend, divisor, value);
    }

    return 0;
}/*}}}*/
#endif
