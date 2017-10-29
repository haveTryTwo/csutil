// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/status.h"
#include "base/cipher.h"
#include "base/random.h"
#include "base/common.h"
#include "base/aes_cipher.h"

#include "test_base/include/test_base.h"

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_10_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data = "abcd1efbh2";
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_32_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 32;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_100_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());
//
//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_1024_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_0_10240_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    // source data
    for (uint32_t i = 0; i < 10240; ++i)
    {
        uint32_t source_data_len = i;
        std::string default_source_data;
        ret = GetRandStr(source_data_len, &default_source_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(source_data_len, default_source_data.size());

        // Encrypt
        std::string encrypt_data;
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);

        // Decrypt
        std::string decrypt_data;
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Exception_Null_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_ECB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/


TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_10_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data = "abcd1efbh2";
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_32_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 32;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_100_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());
//
//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_1024_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_0_10240_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    // source data
    for (uint32_t i = 0; i < 10240; ++i)
    {
        uint32_t source_data_len = i;
        std::string default_source_data;
        ret = GetRandStr(source_data_len, &default_source_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(source_data_len, default_source_data.size());

        // Encrypt
        std::string encrypt_data;
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);

        // Decrypt
        std::string decrypt_data;
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Exception_Null_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/



TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_10_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data = "abcd1efbh2";
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_32_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 32;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_100_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());
//
//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_1024_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_0_10240_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    // source data
    for (uint32_t i = 0; i < 10240; ++i)
    {
        uint32_t source_data_len = i;
        std::string default_source_data;
        ret = GetRandStr(source_data_len, &default_source_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(source_data_len, default_source_data.size());

        // Encrypt
        std::string encrypt_data;
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);

        // Decrypt
        std::string decrypt_data;
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Exception_Null_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CBC);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/



TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_10_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data = "abcd1efbh2";
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_32_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 32;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_100_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());
//
//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_1024_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

//    fprintf(stderr, "encrypt: ");
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
//    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_0_10240_Len)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    // source data
    for (uint32_t i = 0; i < 10240; ++i)
    {
        uint32_t source_data_len = i;
        std::string default_source_data;
        ret = GetRandStr(source_data_len, &default_source_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(source_data_len, default_source_data.size());

        // Encrypt
        std::string encrypt_data;
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);

        // Decrypt
        std::string decrypt_data;
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
    fprintf(stderr, "source:  %s\n", default_source_data.c_str());

    fprintf(stderr, "encrypt: ");
    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);
    fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Exception_Null_Str)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
    // std::string aes_256_key = "ABCDEF0";

    // source data
    uint32_t source_data_len = 0;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
    ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_256->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_256;
    aes_256 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string decrypt_data;
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand)
{/*{{{*/
    using namespace base;

    // key
    std::string aes_128_key = "ABCDEFGH--128279";

    // source data
    uint32_t source_data_len = 8096;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_OFB);
    ret = aes_128->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = aes_128->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string decrypt_data;
    for (uint32_t i = 0; i < 10000; ++i)
    {
        ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
    }
    EXPECT_EQ(default_source_data, decrypt_data);

    delete aes_128;
    aes_128 = NULL;
}/*}}}*/
