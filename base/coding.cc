// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unordered_map>
#include <unordered_set>

#include <string.h>

#include "base/util.h"
#include "base/coding.h"
#include "base/common.h"

namespace base
{
typedef Code (*ToVisibleChar)(uint8_t invisible_src, uint8_t *visible_dst);
typedef Code (*ToInvisibleChar)(uint8_t visible_src, uint8_t *invisible_dst);

// Function for base64
static Code GetUnitVisibleChar(const char* unit_str, int size, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t *num4);
static Code GetUnitInvisibleChar(const char* unit_str, int size, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t* invisible_size);
static Code GetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst);
static Code GetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst);


// Function for base32
static Code Base32EncodeInternal(const std::string &src, ToVisibleChar to_visible_char, std::string *dst);
static Code Base32DecodeInternal(const std::string &src, ToInvisibleChar to_invisible_char, std::string *dst);
static Code Base32GetUnitVisibleChar(const char* unit_str, int size, ToVisibleChar to_visible_char, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t *num4, uint8_t *num5, uint8_t *num6, uint8_t *num7, uint8_t *num8);
static Code Base32GetUnitInvisibleChar(const char* unit_str, int size, ToInvisibleChar to_invisible_char, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t* num4, uint8_t* num5, uint8_t* invisible_size);
static Code Base32GetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst);
static Code Base32GetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst);
static Code Base32ForGeoHashGetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst);
static Code Base32ForGeoHashGetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst);
static Code BuildNext(const std::string &needle, std::deque<int> *next);


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
    if (in.size() < sizeof(*value)) return kInvalidLength;

#if (BYTE_ORDER == LITTLE_ENDIAN)
    memcpy(value, in.data(), sizeof(*value));
#else
    for (int i = 0; i < sizeof(*value); ++i)
    {
        *value += *(reinterpret_cast<const uint8_t*>(in.data()+i)) << i*8;
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
    if (in.size() < sizeof(*value)) return kInvalidLength;

#if (BYTE_ORDER == LITTLE_ENDIAN)
    memcpy(value, in.data(), sizeof(*value));
#else
    for (int i = 0; i < sizeof(*value); ++i)
    {
        *value += *(reinterpret_cast<const uint8_t*>(in.data()+i)) << i*8;
    }
#endif

    return kOk;
}/*}}}*/

Code EncodeVar32(uint32_t num, std::string *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;

    int i = 0;
    char buf[5];
    while (num > 0x7f)
    {
        buf[i] = (num&0x7f)|0x80;
        i++;
        num = num>>7;
    }
    buf[i] = num;
    i++;
    out->append(buf, i);

    return kOk;
}/*}}}*/

Code DecodeVar32(const std::string &in, uint32_t *value)
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    *value = 0;
    bool is_complete = false;
    for (int i = 0; i < (int)in.size(); ++i)
    {
        uint8_t ch = *(reinterpret_cast<const uint8_t*>(in.data()+i));
        if ((ch&0x80) != 0)
        {
            *value += ((ch&0x7f) << (7*i)); 
        }
        else 
        {
            *value += (ch << (7*i)); 
            is_complete = true;
            break; // Note: string in may contain other data
        }
    }

    if (!is_complete) return kInvalidLength;

    return kOk;
}/*}}}*/

Code EncodeVar64(uint64_t num, std::string *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;

    int i = 0;
    char buf[10];
    while (num > 0x7f)
    {
        buf[i] = (num&0x7f)|0x80;
        i++;
        num = num>>7;
    }
    buf[i] = num;
    i++;
    out->append(buf, i);

    return kOk;
}/*}}}*/

Code DecodeVar64(const std::string &in, uint64_t *value)
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    *value = 0;
    bool is_complete = false;
    for (int i = 0; i < (int)in.size(); ++i)
    {
        uint8_t ch = *(reinterpret_cast<const uint8_t*>(in.data()+i));
        if ((ch&0x80) != 0)
        {
            *value += (((uint64_t)(ch&0x7f)) << (7*i)); 
        }
        else 
        {
            *value += (((uint64_t)ch) << (7*i)); 
            is_complete = true;
            break; // Note: string in may contain other data
        }
    }

    if (!is_complete) return kInvalidLength;

    return kOk;
}/*}}}*/

Code EncodeZigZag32(int num, uint32_t *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;

    *out = (num<<1)^(num>>31);

    return kOk;
}/*}}}*/

Code DecodeZigZag32(uint32_t in, int *num)
{/*{{{*/
    if (num == NULL) return kInvalidParam;
    
    *num = (in>>1) ^ (-(in&1));

    return kOk;
}/*}}}*/

Code EncodeZigZag64(int64_t num, uint64_t *out)
{/*{{{*/
    if (out == NULL) return kInvalidParam;

    *out = (num<<1)^(num>>63);

    return kOk;
}/*}}}*/

Code DecodeZigZag64(uint64_t in, int64_t *num)
{/*{{{*/
    if (num == NULL) return kInvalidParam;
    
    *num = (in>>1) ^ (-(in&1));

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

/**
 * Base64 encode and decode
 */
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
    if (unit_str == NULL || size != (int)kUnitVisibleSizeOfBase64 || num1 == NULL
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

/**
 * Base16 encode and decode
 */
Code Base16Encode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return GetReadableStr(src, dst);
}/*}}}*/

Code Base16Decode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return GetBinStr(src, dst);
}/*}}}*/

/**
 * Base32 encode and decode
 */
Code Base32Encode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return Base32EncodeInternal(src, Base32GetVisibleChar, dst);
}/*}}}*/

Code Base32Decode(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return Base32DecodeInternal(src, Base32GetInvisibleChar, dst);
}/*}}}*/

Code Base32EncodeForGeoHash(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return Base32EncodeInternal(src, Base32ForGeoHashGetVisibleChar, dst);
}/*}}}*/

Code Base32DecodeForGeoHash(const std::string &src, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    return Base32DecodeInternal(src, Base32ForGeoHashGetInvisibleChar, dst);
}/*}}}*/


Code Base32EncodeInternal(const std::string &src, ToVisibleChar to_visible_char, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    dst->clear();

    if (src.size() == 0) return kOk;

    uint8_t num1 = 0;
    uint8_t num2 = 0;
    uint8_t num3 = 0;
    uint8_t num4 = 0;
    uint8_t num5 = 0;
    uint8_t num6 = 0;
    uint8_t num7 = 0;
    uint8_t num8 = 0;
    uint32_t unit_num = src.size()/kUnitSizeOfBase32;
    uint32_t bytes_left = src.size()%kUnitSizeOfBase32;

    for (uint32_t i = 0; i < unit_num; ++i)
    {
        const char *ptr = src.data()+i*kUnitSizeOfBase32;
        ret = Base32GetUnitVisibleChar(ptr, kUnitSizeOfBase32, to_visible_char, &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8);
        if (ret != kOk) return ret;
        dst->append(1, num1);
        dst->append(1, num2);
        dst->append(1, num3);
        dst->append(1, num4);
        dst->append(1, num5);
        dst->append(1, num6);
        dst->append(1, num7);
        dst->append(1, num8);
    }

    if (bytes_left != 0)
    {
        ret = Base32GetUnitVisibleChar(src.data()+src.size()-bytes_left, bytes_left, to_visible_char, &num1, &num2, &num3, &num4, &num5, &num6, &num7, &num8);
        if (ret != kOk) return ret;
        dst->append(1, num1);
        dst->append(1, num2);
        dst->append(1, num3);
        dst->append(1, num4);
        dst->append(1, num5);
        dst->append(1, num6);
        dst->append(1, num7);
        dst->append(1, num8);
    }

    return ret;
}/*}}}*/

Code Base32DecodeInternal(const std::string &src, ToInvisibleChar to_invisible_char, std::string *dst)
{/*{{{*/
    if (dst == NULL) return kInvalidParam;

    Code ret = kOk;
    dst->clear();

    if (src.size() == 0) return ret;

    uint32_t unit_num = src.size() / kUnitVisibleSizeOfBase32;
    if (src.size() % kUnitVisibleSizeOfBase32 != 0) return kInvalidParam;

    uint8_t num1 = 0;
    uint8_t num2 = 0;
    uint8_t num3 = 0;
    uint8_t num4 = 0;
    uint8_t num5 = 0;
    uint8_t invisible_size = 0;
    for (uint32_t i = 0; i < unit_num; ++i)
    {
        const char *ptr = src.data() + i*kUnitVisibleSizeOfBase32;

        ret = Base32GetUnitInvisibleChar(ptr, kUnitVisibleSizeOfBase32, to_invisible_char, &num1, &num2, &num3, &num4, &num5, &invisible_size);
        if (ret != kOk) return ret;
        if (invisible_size > 5) return kInvalidParam;

        if (invisible_size == 1)
        {
            dst->append(1, num1);
        }
        else if (invisible_size == 2)
        {
            dst->append(1, num1);
            dst->append(1, num2);
        }
        else if (invisible_size == 3)
        {
            dst->append(1, num1);
            dst->append(1, num2);
            dst->append(1, num3);
        }
        else if (invisible_size == 4)
        {
            dst->append(1, num1);
            dst->append(1, num2);
            dst->append(1, num3);
            dst->append(1, num4);
        }
        else
        {
            dst->append(1, num1);
            dst->append(1, num2);
            dst->append(1, num3);
            dst->append(1, num4);
            dst->append(1, num5);
        }
    }

    return ret;
}/*}}}*/

Code Base32GetUnitVisibleChar(const char* unit_str, int size, ToVisibleChar to_visible_char, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t *num4, uint8_t *num5, uint8_t *num6, uint8_t *num7, uint8_t *num8)
{/*{{{*/
    if (unit_str == NULL || size <= 0 || size > (int)kUnitSizeOfBase32
        || num1 == NULL || num2 == NULL || num3 == NULL || num4 == NULL
        || num5 == NULL || num6 == NULL || num7 == NULL || num8 == NULL)
        return kInvalidParam;

    Code ret = kOk;
    uint8_t tmp_num1 = 0;
    uint8_t tmp_num2 = 0;
    uint8_t tmp_num3 = 0;
    uint8_t tmp_num4 = 0;
    uint8_t tmp_num5 = 0;
    uint8_t tmp_num6 = 0;
    uint8_t tmp_num7 = 0;
    uint8_t tmp_num8 = 0;

    if (size == 1)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xf8)>>3;
        tmp_num2 = (uint8_t)(unit_str[0]&0x07)<<2;

        ret = to_visible_char(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num2, num2);
        if (ret != kOk) return ret;
        *num3 = kEqualChar;
        *num4 = kEqualChar;
        *num5 = kEqualChar;
        *num6 = kEqualChar;
        *num7 = kEqualChar;
        *num8 = kEqualChar;
    }
    else if (size == 2)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xf8)>>3;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x07)<<2) | ((uint8_t)(unit_str[1]&0xc0)>>6);
        tmp_num3 = (uint8_t)(unit_str[1]&0x3e)>>1;
        tmp_num4 = (uint8_t)(unit_str[1]&0x01)<<4;

        ret = to_visible_char(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num3, num3);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num4, num4);
        if (ret != kOk) return ret;
        *num5 = kEqualChar;
        *num6 = kEqualChar;
        *num7 = kEqualChar;
        *num8 = kEqualChar;
    }
    else if (size == 3)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xf8)>>3;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x07)<<2) | ((uint8_t)(unit_str[1]&0xc0)>>6);
        tmp_num3 = (uint8_t)(unit_str[1]&0x3e)>>1;
        tmp_num4 = (uint8_t)(unit_str[1]&0x01)<<4 | ((uint8_t)(unit_str[2]&0xf0)>>4);
        tmp_num5 = (uint8_t)(unit_str[2]&0x0f)<<1;

        ret = to_visible_char(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num3, num3);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num4, num4);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num5, num5);
        if (ret != kOk) return ret;
        *num6 = kEqualChar;
        *num7 = kEqualChar;
        *num8 = kEqualChar;
    }
    else if (size == 4)
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xf8)>>3;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x07)<<2) | ((uint8_t)(unit_str[1]&0xc0)>>6);
        tmp_num3 = (uint8_t)(unit_str[1]&0x3e)>>1;
        tmp_num4 = (uint8_t)(unit_str[1]&0x01)<<4 | ((uint8_t)(unit_str[2]&0xf0)>>4);
        tmp_num5 = (uint8_t)(unit_str[2]&0x0f)<<1 | ((uint8_t)(unit_str[3]&0x80)>>7);
        tmp_num6 = (uint8_t)(unit_str[3]&0x7c)>>2;
        tmp_num7 = (uint8_t)(unit_str[3]&0x03)<<3;

        ret = to_visible_char(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num3, num3);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num4, num4);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num5, num5);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num6, num6);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num7, num7);
        if (ret != kOk) return ret;
        *num8 = kEqualChar;
    }
    else
    {
        tmp_num1 = (uint8_t)(unit_str[0]&0xf8)>>3;
        tmp_num2 = ((uint8_t)(unit_str[0]&0x07)<<2) | ((uint8_t)(unit_str[1]&0xc0)>>6);
        tmp_num3 = (uint8_t)(unit_str[1]&0x3e)>>1;
        tmp_num4 = (uint8_t)(unit_str[1]&0x01)<<4 | ((uint8_t)(unit_str[2]&0xf0)>>4);
        tmp_num5 = (uint8_t)(unit_str[2]&0x0f)<<1 | ((uint8_t)(unit_str[3]&0x80)>>7);
        tmp_num6 = (uint8_t)(unit_str[3]&0x7c)>>2;
        tmp_num7 = (uint8_t)(unit_str[3]&0x03)<<3 | ((uint8_t)(unit_str[4]&0xe0)>>5);
        tmp_num8 = (uint8_t)(unit_str[4]&0x1f);

        ret = to_visible_char(tmp_num1, num1);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num2, num2);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num3, num3);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num4, num4);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num5, num5);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num6, num6);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num7, num7);
        if (ret != kOk) return ret;
        ret = to_visible_char(tmp_num8, num8);
        if (ret != kOk) return ret;
    }

    return kOk;
}/*}}}*/

Code Base32GetUnitInvisibleChar(const char* unit_str, int size, ToInvisibleChar to_invisible_char, uint8_t* num1, uint8_t* num2, uint8_t* num3, uint8_t* num4, uint8_t* num5, uint8_t* invisible_size)
{/*{{{*/
    if (unit_str == NULL || size != (int)kUnitVisibleSizeOfBase32 || num1 == NULL
        || num2 == NULL || num3 == NULL || num4 == NULL || num5 == NULL || invisible_size == NULL)
        return kInvalidParam; 

    Code ret = kOk;
    uint8_t tmp_num1 = 0;
    uint8_t tmp_num2 = 0;
    uint8_t tmp_num3 = 0;
    uint8_t tmp_num4 = 0;
    uint8_t tmp_num5 = 0;
    uint8_t tmp_num6 = 0;
    uint8_t tmp_num7 = 0;
    uint8_t tmp_num8 = 0;

    if (memcmp(unit_str+2, "======", 6) == 0)
    {/*{{{*/
        ret = to_invisible_char(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x1f)<<3) | ((tmp_num2&0x1f)>>2);
        *invisible_size = 1;
        return ret;
    }/*}}}*/
    else if (memcmp(unit_str+4, "====", 4) == 0)
    {/*{{{*/
        ret = to_invisible_char(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[3], &tmp_num4);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x1f)<<3) | ((tmp_num2&0x1f)>>2);
        *num2 = ((tmp_num2&0x1f)<<6) | ((tmp_num3&0x1f)<<1) | ((tmp_num4&0x1f)>>4);
        *invisible_size = 2;
        return ret;
    }/*}}}*/
    else if (memcmp(unit_str+5, "===", 3) == 0)
    {/*{{{*/
        ret = to_invisible_char(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[3], &tmp_num4);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[4], &tmp_num5);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x1f)<<3) | ((tmp_num2&0x1f)>>2);
        *num2 = ((tmp_num2&0x1f)<<6) | ((tmp_num3&0x1f)<<1) | ((tmp_num4&0x1f)>>4);
        *num3 = ((tmp_num4&0x1f)<<4) | ((tmp_num5&0x1f)>>1);
        *invisible_size = 3;
        return ret;
    }/*}}}*/
    else if (memcmp(unit_str+7, "=", 1) == 0)
    {/*{{{*/
        ret = to_invisible_char(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[3], &tmp_num4);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[4], &tmp_num5);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[5], &tmp_num6);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[6], &tmp_num7);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x1f)<<3) | ((tmp_num2&0x1f)>>2);
        *num2 = ((tmp_num2&0x1f)<<6) | ((tmp_num3&0x1f)<<1) | ((tmp_num4&0x1f)>>4);
        *num3 = ((tmp_num4&0x1f)<<4) | ((tmp_num5&0x1f)>>1);
        *num4 = ((tmp_num5&0x1f)<<7) | ((tmp_num6&0x1f)<<2) | ((tmp_num7&0x1f)>>3);
        *invisible_size = 4;
        return ret;
    }/*}}}*/
    else
    {/*{{{*/
        ret = to_invisible_char(unit_str[0], &tmp_num1);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[1], &tmp_num2);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[2], &tmp_num3);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[3], &tmp_num4);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[4], &tmp_num5);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[5], &tmp_num6);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[6], &tmp_num7);
        if (ret != kOk) return ret;
        ret = to_invisible_char(unit_str[7], &tmp_num8);
        if (ret != kOk) return ret;
        *num1 = ((tmp_num1&0x1f)<<3) | ((tmp_num2&0x1f)>>2);
        *num2 = ((tmp_num2&0x1f)<<6) | ((tmp_num3&0x1f)<<1) | ((tmp_num4&0x1f)>>4);
        *num3 = ((tmp_num4&0x1f)<<4) | ((tmp_num5&0x1f)>>1);
        *num4 = ((tmp_num5&0x1f)<<7) | ((tmp_num6&0x1f)<<2) | ((tmp_num7&0x1f)>>3);
        *num5 = ((tmp_num7&0x1f)<<5) | ((tmp_num8&0x1f));
        *invisible_size = 5;
        return ret;
    }/*}}}*/

    return ret;
}/*}}}*/

Code Base32GetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst)
{/*{{{*/
    if (invisible_src >= 32) return kInvalidParam;

    if (invisible_src >= 0 && invisible_src <= 25)
        *visible_dst = invisible_src + 'A';
    else if (invisible_src >= 26 && invisible_src <= 31)
        *visible_dst = invisible_src - 26 + '2';

    return kOk;
}/*}}}*/

Code Base32GetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst)
{/*{{{*/
    if (visible_src >= 'A' && visible_src <= 'Z')
        *invisible_dst = visible_src - 'A';
    else if (visible_src >= '2' && visible_src <= '7')
        *invisible_dst = visible_src - '2' + 26;
    else
        return kInvalidParam;

    return kOk;
}/*}}}*/

Code Base32ForGeoHashGetVisibleChar(uint8_t invisible_src, uint8_t *visible_dst)
{/*{{{*/
    if (invisible_src >= 32) return kInvalidParam;

    if (invisible_src >= 0 && invisible_src <= 9)
        *visible_dst = invisible_src + '0';
    else if (invisible_src >= 10 && invisible_src <= 16)
        *visible_dst = invisible_src - 10 + 'b';
    else if (invisible_src >= 17 && invisible_src <= 18)
        *visible_dst = invisible_src - 17 + 'j';
    else if (invisible_src >= 19 && invisible_src <= 20)
        *visible_dst = invisible_src - 19 + 'm';
    else if (invisible_src >= 21 && invisible_src <= 31)
        *visible_dst = invisible_src - 21 + 'p';

    return kOk;
}/*}}}*/

Code Base32ForGeoHashGetInvisibleChar(uint8_t visible_src, uint8_t *invisible_dst)
{/*{{{*/
    if (visible_src >= '0' && visible_src <= '9')
        *invisible_dst = visible_src - '0';
    else if (visible_src >= 'b' && visible_src <= 'h')
        *invisible_dst = visible_src - 'b' + 10;
    else if (visible_src >= 'j' && visible_src <= 'k')
        *invisible_dst = visible_src - 'j' + 17;
    else if (visible_src >= 'm' && visible_src <= 'n')
        *invisible_dst = visible_src - 'm' + 19;
    else if (visible_src >= 'p' && visible_src <= 'z')
        *invisible_dst = visible_src - 'p' + 21;
    else
        return kInvalidParam;

    return kOk;
}/*}}}*/


Code BruteForce(const std::string &haystack, const std::string &needle, int *pos)
{/*{{{*/
    if (pos == NULL) return kInvalidParam;

    if (needle.size() > haystack.size()) {
        return kNotFound;
    }

    for (size_t i = 0; i <= haystack.size()-needle.size(); ++i)
    {
        size_t j = 0;
        for (j = 0; j < needle.size(); ++j)
        {
            if (haystack[i+j] != needle[j])
            {
                break;
            }
        }
        if (j == needle.size())
        {
            *pos = i;
            return kOk;
        }
    }

    return kNotFound;
}/*}}}*/

static Code BuildNext(const std::string &needle, std::deque<int> *next)
{/*{{{*/
    if (next == NULL) return kInvalidParam;

    (*next)[0] = -1;
    int i = 0;
    int j = -1;
    while (i < (int)needle.size())
    {
        if (j == -1 || needle[i] == needle[j])
        {
            ++i;
            ++j;
            (*next)[i] = j;
        }
        else
        {
            j = (*next)[j];
        }
    }

    return kOk;
}/*}}}*/

Code KMP(const std::string &haystack, const std::string &needle, int *pos)
{/*{{{*/
    if (pos == NULL) return kInvalidParam;

    if (needle.size() > haystack.size()) {
        return kNotFound;
    }

    std::deque<int> next(needle.size());
    Code ret = BuildNext(needle, &next);
    if (ret != kOk) return ret;

    int i = 0;
    int j = 0;
    while (i < (int)haystack.size() && j < (int)needle.size())
    {
        if (j == -1 || haystack[i] == needle[j])
        {
            ++i;
            ++j;
        }
        else
        {
            j = next[j];
        }
    }

    if (j == (int)needle.size())
    {
        *pos = i-j;
        return kOk;
    }
    return  kNotFound;
}/*}}}*/

Code BM(const std::string &haystack, const std::string &needle, int *pos)
{/*{{{*/
    if (pos == NULL) return kInvalidParam;

    if (needle.size() > haystack.size()) {
        return kNotFound;
    }

    std::unordered_map<char, int> bm_map;
    for (int k = 0; k < (int)needle.size(); ++k)
    {
        bm_map[needle[k]] = k;
    }

    int i = 0;
    int j = 0;
    std::unordered_map<char, int>::const_iterator iter;
    while (i <= (int)(haystack.size() - needle.size()))
    {
        for (j = (int)(needle.size()-1); j >= 0; --j)
        {
            if (needle[j] != haystack[i+j])
            {
                if (bm_map.cend() != (iter = bm_map.find(haystack[i+j])))
                {
                    i += j - iter->second;
                }
                else
                {
                    i += j+1;
                }
                break;
            }
        }

        if (j < 0)
        {
            *pos = i;
            return kOk;
        }
    }

    return kNotFound;
}/*}}}*/

Code RK(const std::string &haystack, const std::string &needle, int *pos)
{/*{{{*/
    if (pos == NULL) return kInvalidParam;

    if (needle.size() > haystack.size()) {
        return kNotFound;
    }

    std::unordered_set<std::string> hash;
    hash.insert(needle);

    for (int i = 0; i <= (int)(haystack.size() - needle.size()); ++i)
    {
        std::string substr = haystack.substr(i, needle.size());

        if (hash.find(substr) == hash.end())
        { // NOTE:htt, if not found, then continue
        }
        else
        {
            *pos = i;
            return kOk;
        }
    }

    return kNotFound;
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
