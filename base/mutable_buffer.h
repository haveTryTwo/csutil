// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SOCK_MUTABLE_BUFFER_H_
#define SOCK_MUTABLE_BUFFER_H_

#include <stdint.h>

#include "base/status.h"

namespace base
{

/**
 * NOTE: MutableBuffer is used for socket connecting when receiving data; 
 * The buffer would be free when it's empty and size is larger then safe size;
 */
class MutableBuffer
{
    public:
        MutableBuffer(uint32_t safe_buf_size = 65536); // default: 64KB
        ~MutableBuffer();

        Code Append(const char *data, uint32_t data_len);
        Code Skip(uint32_t skip_len);
        const char* RealDataPtr();
        uint32_t RealDataSize();

    private:
        MutableBuffer(const MutableBuffer &);
        MutableBuffer& operator = (const MutableBuffer &);

    private:
        /*
         * |----------        buf_size_     --------------|
         * |---offset_---|---cur_buf_size_---|--left_len--|
         */
        char *buf_;         // buf: 
        uint32_t buf_size_;      // the whole buf size
        uint32_t cur_buf_size_;   // real data size
        uint32_t offset_;        // the no used data
        uint32_t safe_buf_size_; // whole data size should be less than safe_buf_size which default 64KB
};

}

#endif
