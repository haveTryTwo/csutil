// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "base/common.h"
#include "base/bit_arr.h"

namespace base
{

BitArr::BitArr(uint32_t bits_num): bits_num_(bits_num), bits_(NULL), is_init_(false)
{/*{{{*/
}/*}}}*/

BitArr::~BitArr()
{/*{{{*/
    if (bits_ != NULL)
    {
        delete [] bits_;
        bits_ = NULL;
    }
}/*}}}*/

Code BitArr::Init()
{/*{{{*/
    if (bits_num_ == 0) return kInvalidParam;

    uint32_t bytes_num = (bits_num_ + kBitsPerByte-1)/kBitsPerByte;
    bits_ = new char[bytes_num];
    if (bits_ == NULL) return kNewFailed;
    memset(bits_, 0, bytes_num);
    is_init_ = true;

    return kOk;
}/*}}}*/

Code BitArr::Put(uint32_t pos, bool value)
{/*{{{*/
    if (pos >= bits_num_) return kInvalidParam;
    if (!is_init_) return kNotInit;

    uint32_t bytes_pos = (bits_num_ - 1 - pos)/kBitsPerByte;
    uint32_t bits_pos = (bits_num_ - 1 - pos)%kBitsPerByte;
    uint32_t bits_value = 1 << (kBitsPerByte-bits_pos-1);
    if (value)
    {
        bits_[bytes_pos] |= bits_value; 
    }
    else
    {
        bits_value = ~bits_value;
        bits_[bytes_pos] &= bits_value;
    } 

    return kOk;
}/*}}}*/

Code BitArr::Get(uint32_t pos, bool *value)
{/*{{{*/
    if (pos >= bits_num_ || value == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    *value = false;

    uint32_t bytes_pos = (bits_num_ - 1 - pos)/kBitsPerByte;
    uint32_t bits_pos = (bits_num_ - 1 - pos)%kBitsPerByte;
    uint32_t bits_value = 1 << (kBitsPerByte-bits_pos-1);
    uint8_t byte = (bits_[bytes_pos] & bits_value);
    if (byte != 0)
    {
        *value = true;
    }

    return kOk;
}/*}}}*/

Code BitArr::Clear()
{/*{{{*/
    if (!is_init_) return kNotInit;

    uint32_t bytes_num = (bits_num_ + kBitsPerByte-1)/kBitsPerByte;
    memset(bits_, 0, bytes_num);

    return kOk;
}/*}}}*/

Code BitArr::Size(uint32_t *size)
{/*{{{*/
    if (size == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    *size = bits_num_;

    return kOk;
}/*}}}*/

Code BitArr::ToString(std::string *str)
{/*{{{*/
    if (str == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    for (int pos = bits_num_-1; pos >= 0; --pos)
    {
        bool value = false;
        Code ret = Get(pos, &value);
        if (ret != kOk) return ret;

        if (value) 
        {
            str->append(1, '1');
        }
        else
        {
            str->append(1, '0');
        }
    }

    return kOk;
}/*}}}*/

}
