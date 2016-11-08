// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "base/int.h"

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
    long long v = strtoull(str.c_str(), &end_ptr, base);
    if ((errno == ERANGE && (v == LONG_MAX || v == LONG_MIN)) ||
            (errno != 0 && v == 0))
        return kStrtollFailed;
    if (end_ptr == str.c_str()) return kNoDigits;
    if (*end_ptr != '\0') return kNotAllDigits;

    *num = v;
    return kOk;
}/*}}}*/

Code GetUInt64(const std::string &str, uint64_t *num, int base)
{/*{{{*/
    if (num == NULL) return kInvalidParam;
    if (base < 0 || base > 36) return kInvalidParam;

    errno = 0;
    char *end_ptr = NULL;
    unsigned long long v = strtoull(str.c_str(), &end_ptr, base);
    if ((errno == ERANGE && (v == LONG_MAX || v == LONG_MIN)) ||
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

Code BigAdd(const std::string &ln, const std::string &rn, std::string *reslut)
{/*{{{*/
    if (reslut == NULL) return kInvalidParam;

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
