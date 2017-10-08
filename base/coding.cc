// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "base/coding.h"
#include "base/common.h"

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

Code EncodeFixed64(uint64_t num, std::string *out)
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

Code DecodeFixed64(const std::string &in, uint64_t *value)
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

Code Base64Encode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    dst->clear();

    if (src.size() == 0) return kOk;

    uint8_t num1 = 0;
    uint8_t num2 = 0;
    uint8_t num3 = 0;
    uint8_t num4 = 0;
    uint32_t unit_num = src.size()/kUnitSizeOfBase64;
    uint32_t bytes_left = src.size()%kUnitSizeOfBase64;

    for (uint32_t i = 0; i < unit_num; ++i)
    {
        const char *ptr = src.data()+i*kUnitSizeOfBase64;
        ret = GetUnitVisibleChar(ptr, kUnitSizeOfBase64, &num1, &num2, &num3, &num4);
        if (ret != kOk) return ret;
        dst->append(1, num1);
        dst->append(1, num2);
        dst->append(1, num3);
        dst->append(1, num4);
    }

    if (bytes_left != 0)
    {
        ret = GetUnitVisibleChar(src.data()+src.size()-bytes_left, bytes_left, &num1, &num2, &num3, &num4);
        if (ret != kOk) return ret;
        dst->append(1, num1);
        dst->append(1, num2);
        dst->append(1, num3);
        dst->append(1, num4);
    }

    return ret;
}/*}}}*/

Code Base64Decode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    dst->clear();

    if (src.size() == 0) return ret;

    uint32_t unit_num = src.size() / kUnitVisibleSizeOfBase64;
    if (src.size() % kUnitVisibleSizeOfBase64 != 0) return kInvalidParam;

    uint8_t num1 = 0;
    uint8_t num2 = 0;
    uint8_t num3 = 0;
    uint8_t invisible_size = 0;
    for (uint32_t i = 0; i < unit_num; ++i)
    {
        const char *ptr = src.data() + i*kUnitVisibleSizeOfBase64;

        ret = GetUnitInvisibleChar(ptr, kUnitVisibleSizeOfBase64, &num1, &num2, &num3, &invisible_size);
        if (ret != kOk) return ret;
        if (invisible_size == 1)
        {
            dst->append(1, num1);
        }
        else if (invisible_size == 2)
        {
            dst->append(1, num1);
            dst->append(1, num2);
        }
        else
        {
            dst->append(1, num1);
            dst->append(1, num2);
            dst->append(1, num3);
        }
    }

    return ret;
}/*}}}*/

Code GetUnitVisibleChar(const char* unit_str, int size, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t *num4)
{/*{{{*/
    if (unit_str == NULL || size <= 0 || size > (int)kUnitSizeOfBase64 
        || num1 == NULL || num2 == NULL || num3 == NULL || num4 == NULL)
        return kInvalidParam;

    Code ret = kOk;
    uint8_t tmp_num1 = 0;
    uint8_t tmp_num2 = 0;
    uint8_t tmp_num3 = 0;
    uint8_t tmp_num4 = 0;

    if (size == 1)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xfc)>>2;
        tmp_num2 = (uint8_t)(unit_str[0]&0x03)<<4;

        ret = GetVisibleChar(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num2, num2);
        if (ret != kOk) return ret;
        *num3 = kEqualChar;
        *num4 = kEqualChar;
    }
    else if (size == 2)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xfc)>>2;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x03)<<4) | ((uint8_t)(unit_str[1]&0xf0)>>4);
        tmp_num3 = (uint8_t)(unit_str[1]&0x0f)<<2;

        ret = GetVisibleChar(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num3, num3);
        if (ret != kOk) return ret;
        *num4 = kEqualChar;
    }
    else
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xfc)>>2;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x03)<<4) | ((uint8_t)(unit_str[1]&0xf0)>>4);
        tmp_num3 = ((uint8_t)unit_str[1]&0x0f)<<2 | (((uint8_t)unit_str[2]&0xc0)>>6);
        tmp_num4 = (uint8_t)(unit_str[2]&0x3f);

        ret = GetVisibleChar(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num3, num3);
        if (ret != kOk) return ret;
        ret = GetVisibleChar(tmp_num4, num4);
        if (ret != kOk) return ret;
    }

    return kOk;
}/*}}}*/

Code GetUnitInvisibleChar(const char* unit_str, int size, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t* invisible_size)
{/*{{{*/
    if (unit_str == NULL || size != kUnitVisibleSizeOfBase64 || num1 == NULL
        || num2 == NULL || num3 == NULL || invisible_size == NULL)
        return kInvalidParam; 

    Code ret = kOk;
    uint8_t tmp_num1 = 0;
    uint8_t tmp_num2 = 0;
    uint8_t tmp_num3 = 0;
    uint8_t tmp_num4 = 0;
    if ((uint8_t)unit_str[2] == kEqualChar && (uint8_t)unit_str[3] == kEqualChar)
    {
        ret = GetInvisibleChar(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x3f)<<2) | ((tmp_num2&0x3f)>>4);
        *invisible_size = 1;
        return ret;
    }
    else if ((uint8_t)unit_str[3] == kEqualChar)
    {
        ret = GetInvisibleChar(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x3f)<<2) | ((tmp_num2&0x3f)>>4);
        *num2 = ((tmp_num2&0x3f)<<4) | ((tmp_num3&0x3f)>>2);
        *invisible_size = 2;
        return ret;
    }
    else
    {
        ret = GetInvisibleChar(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        ret = GetInvisibleChar(unit_str[3], &tmp_num4);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x3f)<<2) | ((tmp_num2&0x3f)>>4);
        *num2 = ((tmp_num2&0x3f)<<4) | ((tmp_num3&0x3f)>>2);
        *num3 = ((tmp_num3&0x3f)<<6) | (tmp_num4&0x3f);
        *invisible_size = 3;
        return ret;
    }

    return ret;
}/*}}}*/

Code GetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst)
{/*{{{*/
    if (invisible_src >= 64) return kInvalidParam;

    if (invisible_src >= 0 && invisible_src <= 25)
        *visible_dst = invisible_src + 'A';
    else if (invisible_src >= 26 && invisible_src <= 51)
        *visible_dst = invisible_src - 26 + 'a';
    else if (invisible_src >= 52 && invisible_src <= 61)
        *visible_dst = invisible_src - 52 + '0';
    else if (invisible_src == 62)
        *visible_dst = '+';
    else if (invisible_src == 63)
        *visible_dst = '/';

    return kOk;
}/*}}}*/

Code GetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst)
{/*{{{*/
    if (visible_src >= 'A' && visible_src <= 'Z')
        *invisible_dst = visible_src - 'A';
    else if (visible_src >= 'a' && visible_src <= 'z')
        *invisible_dst = visible_src - 'a' + 26;
    else if (visible_src >= '0' && visible_src <= '9')
        *invisible_dst = visible_src - '0' + 52;
    else if (visible_src == '+')
        *invisible_dst = 62;
    else if (visible_src == '/')
        *invisible_dst = 63;
    else
        return kInvalidParam;

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
