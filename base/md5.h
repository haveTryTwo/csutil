// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MD5_H_ 
#define BASE_MD5_H_

#include <string>

#include <stdint.h>
#include <sys/cdefs.h>

#include "base/status.h"
#include "base/message_digest.h"

namespace base
{

class Md5 : public MessageDigest
{
    public:
        Md5();
        virtual ~Md5();

    public:
        virtual Code Init();
        // Note: dest size is 16Byte(128bit), which is binary
        virtual Code Sum(const std::string &content, std::string *dest);

    private:
        Code Update(const char *content, uint32_t len);
        Code Final(uint8_t digest[16]);
        Code Pad();
        Code Transform(const uint8_t block[64], uint32_t state[4]);
        Code Encode32N(const uint32_t *source, uint32_t len, uint8_t *dest);
        Code Decode32N(const uint8_t *source, uint32_t len, uint32_t *dest);
        Code Clear();

    private:
        uint32_t state_[4];
        uint32_t count_[2];
        uint8_t buffer_[64];
        bool is_init_;
};

}

#endif
