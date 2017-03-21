// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/status.h"
#include "base/cipher.h"
#include "base/random.h"
#include "base/common.h"
#include "base/rsa_cipher.h"

#include "test_base/include/test_base.h"

TEST(Cipher, Test_Encrpyt_Decrypt_Normal_10_Len)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

//    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Normal_100_Len)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

//    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Normal_1024_Len)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

//    fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
//    for (uint32_t i = 0; i < encrypt_data.size(); ++i)
//    {
//        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
//    }
//    fprintf(stderr, "\n");

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    std::string default_source_data = "";

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    Code ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Exception_Null_Str)
{/*{{{*/
    using namespace base;

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    Code ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string default_source_data = "abc edf";
    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, NULL);
    EXPECT_EQ(kInvalidParam, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Encrypt_10_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Decrypt_10_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Encrypt_100_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Decrypt_100_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Encrypt_1024_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
        EXPECT_EQ(kOk, ret);
    }

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(default_source_data, decrypt_data);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Encrpyt_Decrypt_Press_Decrypt_1024_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Encrypt
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string encrypt_data;
    ret = pub_rsa->Encrypt(default_source_data, &encrypt_data);
    EXPECT_EQ(kOk, ret);

    // Decrypt
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string decrypt_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Decrypt(encrypt_data, &decrypt_data);
        EXPECT_EQ(kOk, ret);
        EXPECT_EQ(default_source_data, decrypt_data);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Normal)
{/*{{{*/
    using namespace base;

    std::string default_source_data = "my name is haveTryTwo";

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    Code ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "sign_data size:%zu\n", sign_data.size());
    for (uint32_t i = 0; i < sign_data.size(); ++i)
    {
        fprintf(stderr, "%02x", (unsigned char)sign_data.data()[i]);
    }
    fprintf(stderr, "\n");

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    ret = pub_rsa->Verify(default_source_data, sign_data);
    EXPECT_EQ(kOk, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Sign_10_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Sign(default_source_data, &sign_data);
        EXPECT_EQ(kOk, ret);
    }

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    ret = pub_rsa->Verify(default_source_data, sign_data);
    EXPECT_EQ(kOk, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Verify_10_Len_TenThousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 10;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    for (uint32_t i = 0; i < (uint32_t)kThousand*10; ++i)
    {
        ret = pub_rsa->Verify(default_source_data, sign_data);
        EXPECT_EQ(kOk, ret);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Sign_100_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Sign(default_source_data, &sign_data);
        EXPECT_EQ(kOk, ret);
    }

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    ret = pub_rsa->Verify(default_source_data, sign_data);
    EXPECT_EQ(kOk, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Verify_100_Len_TenThousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 100;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    for (uint32_t i = 0; i < (uint32_t)kThousand*10; ++i)
    {
        ret = pub_rsa->Verify(default_source_data, sign_data);
        EXPECT_EQ(kOk, ret);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Sign_1024_Len_Thousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    for (uint32_t i = 0; i < (uint32_t)kThousand; ++i)
    {
        ret = pri_rsa->Sign(default_source_data, &sign_data);
        EXPECT_EQ(kOk, ret);
    }

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    ret = pub_rsa->Verify(default_source_data, sign_data);
    EXPECT_EQ(kOk, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Press_Verify_1024_Len_TenThousand)
{/*{{{*/
    using namespace base;

    uint32_t source_data_len = 1024;
    std::string default_source_data;
    Code ret = GetRandStr(source_data_len, &default_source_data);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(source_data_len, default_source_data.size());

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    for (uint32_t i = 0; i < (uint32_t)kThousand*10; ++i)
    {
        ret = pub_rsa->Verify(default_source_data, sign_data);
        EXPECT_EQ(kOk, ret);
    }

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Exception_Empty_Str)
{/*{{{*/
    using namespace base;

    std::string default_source_data = "";

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    Code ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    ret = pub_rsa->Verify(default_source_data, sign_data);
    EXPECT_EQ(kOk, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/

TEST(Cipher, Test_Sign_Verify_Exception_Wrong_Str)
{/*{{{*/
    using namespace base;

    std::string default_source_data = "my name is haveTryTwo";

    // Sign
    std::string pri_rsa_key_path = "../etc/rsa_private_key.pem";
    Cipher *pri_rsa = new PriRsaCipher(pri_rsa_key_path);
    Code ret = pri_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string sign_data;
    ret = pri_rsa->Sign(default_source_data, &sign_data);
    EXPECT_EQ(kOk, ret);

    // Verify 
    std::string pub_rsa_key_path = "../etc/rsa_public_key.pem";
    Cipher *pub_rsa = new PubRsaCipher(pub_rsa_key_path);
    ret = pub_rsa->Init();
    EXPECT_EQ(kOk, ret);

    std::string source_data = "aa";
    ret = pub_rsa->Verify(source_data, sign_data);
    EXPECT_EQ(kRSAVerifyFailed, ret);

    delete pub_rsa;
    pub_rsa = NULL;

    delete pri_rsa;
    pri_rsa = NULL;
}/*}}}*/
