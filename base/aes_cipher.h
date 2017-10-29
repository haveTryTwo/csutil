// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_AES_CIPHER_H_ 
#define BASE_AES_CIPHER_H_

#include "base/status.h"
#include "base/cipher.h"

#include <openssl/evp.h>
#include <openssl/aes.h>

namespace base
{

enum AES_KEY_LEN_FLAG
{
    AES_128_ECB,
    AES_192_ECB,
    AES_256_ECB,
    AES_128_CBC,
    AES_192_CBC,
    AES_256_CBC,
    AES_128_CFB,
    AES_192_CFB,
    AES_256_CFB,
    AES_128_OFB,
    AES_192_OFB,
    AES_256_OFB,
};

class AESCipher : public Cipher
{
    public:
        AESCipher(std::string key, int key_len_flag);
        virtual ~AESCipher();

    public:
        virtual Code Init();

    public:
        virtual Code Encrypt(const std::string &source_data, std::string *encrpyt_data);
        virtual Code Decrypt(const std::string &encrypt_data, std::string *source_data);

    private:
        std::string key_;
        std::string iv_;
        int key_len_flag_;
        const EVP_CIPHER *evp_cipher_;
        bool is_init_;
};

}

#endif

