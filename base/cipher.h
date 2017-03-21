// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CIPHER_H_ 
#define BASE_CIPHER_H_

#include "base/status.h"

namespace base
{

class Cipher
{
    public:
        virtual ~Cipher() {};

    public:
        virtual Code Init();

    public:
        virtual Code Encrypt(const std::string &source_data, std::string *encrpyt_data);
        virtual Code Decrypt(const std::string &encrypt_data, std::string *source_data);

        virtual Code Sign(const std::string &source_data, std::string *sign_data);
        virtual Code Verify(const std::string &soruce_data, const std::string &sign_data);

    protected:
        Cipher() {}
};

}

#endif
