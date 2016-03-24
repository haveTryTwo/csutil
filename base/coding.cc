// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "base/coding.h"

namespace base
{

Code EncodeFixed32(uint32_t num, std::string *out)
{/*{{{*/
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
}/*}}}*/

Code DecodeFixed32(const std::string &in, uint32_t *value)
{/*{{{*/
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
}/*}}}*/

Code ToHex(uint8_t src_ch, uint8_t *dst_hex_ch)
{/*{{{*/
    if (dst_hex_ch == NULL) return kInvalidParam;

    if ((src_ch & 0x0f) > 0x09)
        *dst_hex_ch = 'a' + (src_ch & 0x0f);
    else
        *dst_hex_ch = '0' + (src_ch & 0x0f);

    return kOk;
}/*}}}*/

Code ToBin(uint8_t src_hex_ch, uint8_t *dst_ch)
{/*{{{*/
    if (dst_ch == NULL) return kInvalidParam;

    if (src_hex_ch >= 'a' && src_hex_ch <= 'z')
        *dst_ch = src_hex_ch - 'a';
    else if (src_hex_ch >= 'A' && src_hex_ch <= 'Z')
        *dst_ch = src_hex_ch - 'A';
    else if (src_hex_ch >= '0' && src_hex_ch <= '9')
        *dst_ch = src_hex_ch - '0';
    else
        return kInvalidParam;

    return kOk;
}/*}}}*/

Code BinToAscii(uint8_t src_ch, uint8_t *dst_high_ch, uint8_t *dst_low_ch)
{/*{{{*/
    if (dst_high_ch == NULL || dst_low_ch == NULL) return kInvalidParam;

    ToHex(src_ch & 0x0f, dst_low_ch);
    ToHex(src_ch >> 4, dst_high_ch);

    return kOk;
}/*}}}*/

Code AsciiToBin(uint8_t src_high_ch, uint8_t src_low_ch, uint8_t *dst_bin_ch)
{/*{{{*/
    if (dst_bin_ch == NULL) return kInvalidParam;

    uint8_t tmp = 0;
    Code ret = ToBin(src_high_ch, &tmp);
    if (ret != kOk) return ret;
    *dst_bin_ch += tmp << 4;
    
    ret = ToBin(src_low_ch, &tmp);
    if (ret != kOk) return ret;
    *dst_bin_ch += tmp;

    return kOk;
}/*}}}*/

Code UrlEncode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    for (int i = 0; i < (int)src.size(); ++i)
    {
        if (isalnum(src.data()[i]) || src.data()[i] == '-' || src.data()[i] == '_' ||
            src.data()[i] == '.' || src.data()[i] == '~')
        {   // unreserved characters
            dst->append(1, src.data()[i]);
        }
        else if (src.data()[i] == ' ')
        {
            dst->append(1, '+');
        }
        else
        {   // reserved characters
            dst->append(1, '%');            
            uint8_t src_high_ch = 0;
            uint8_t src_low_ch = 0;
            ret = BinToAscii(src.data()[i], &src_high_ch, &src_low_ch);
            if (ret != kOk) return ret;

            dst->append(1, src_high_ch);
            dst->append(1, src_low_ch);
        }
    }

    return kOk;
}/*}}}*/

Code UrlDecode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    for (int i = 0; i < (int)src.size(); ++i)
    {
        if (src.data()[i] == '+')
        {
            dst->append(1, ' ');
        }
        else if (src.data()[i] == '%')
        {
            if (i + 2 >= (int)src.size()) return kInvalidUrlEncodingStr;

            uint8_t bin_ch = 0;
            ret = AsciiToBin(src.data()[i+1], src.data()[i+2], &bin_ch);
            if (ret != kOk) return ret;
            dst->append(1, bin_ch);
            i += 2;
        }
        else
        {
            dst->append(1, src.data()[i]);
        }
    }

    return kOk;
}/*}}}*/

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

    std::string src_str = "zhangwang1235-_.~ #@+a张三李四";
    fprintf(stderr, "src_str:     %s\n", src_str.c_str());

    std::string encoding_str;
    UrlEncode(src_str, &encoding_str);
    fprintf(stderr, "encoding_str:%s\n", encoding_str.c_str());
    std::string encoding_str_again;
    UrlEncode(encoding_str, &encoding_str_again);
    fprintf(stderr, "encoding_str_again:%s\n", encoding_str_again.c_str());

    std::string decoding_str;
    UrlDecode(encoding_str_again, &decoding_str);
    fprintf(stderr, "decoding_str:%s\n", decoding_str.c_str());
    std::string decoding_str_again;
    UrlDecode(decoding_str, &decoding_str_again);
    fprintf(stderr, "decoding_str_again:%s\n", decoding_str_again.c_str());

    if (src_str == decoding_str_again)
        fprintf(stderr, "[EQUAL] src_str == decoding_str_again\n");
    else
        fprintf(stderr, "[NOT EQUAL] src_str != decoding_str_again\n");

    return 0;
}/*}}}*/
#endif
