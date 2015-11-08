// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "base/coding.h"

namespace base
{

Code EncodeFixed32(uint32_t num, std::string *out)
{
    if (out == NULL) return kInvalidParam;

    char buf[sizeof(num)];
#if (BYTE_ORDER == LITTLE_ENDIAN)
    memcpy(buf, &num, sizeof(buf));
    out->append(buf, sizeof(buf));
#else
    for (int i = 0; i < (int)sizeof(num); ++i)
    {
        buf[i] = (num >> i*8) & 0xff;
    }
    out->append(buf, sizeof(buf));
#endif

    return kOk;
}

Code DecodeFixed32(const std::string &in, uint32_t *value)
{
    if (value == NULL) return kInvalidParam;

#if (BYTE_ORDER == LITTLE_ENDIAN)
    memcpy(value, in.data(), sizeof(*value));
#else
    for (int i = 0; i < sizeof(*value); ++i)
    {
        *value += (reinterpret_cast<uint8_t*>(in->data()+i)) << i*8;
    }
#endif

    return kOk;
}

}

#ifdef _CODING_MAIN_TEST_
#include <stdio.h>
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

    int num = 0xaabbccdd;
    std::string encode_str;
    EncodeFixed32(num, &encode_str);

    int out_num;
    DecodeFixed32(encode_str, reinterpret_cast<uint32_t*>(&out_num));
    fprintf(stderr, "before num:%#x, after num:%#x\n", num, out_num);

    return 0;
}/*}}}*/
#endif
