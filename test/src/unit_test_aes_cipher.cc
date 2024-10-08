// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/aes_cipher.h"
#include "base/cipher.h"
#include "base/coding.h"
#include "base/common.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_NewTest_Len) { /*{{{*/
  using namespace base;

  // key
  //    std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
  //    std::string aes_256_key = "mmpayuserrollapi";
  std::string aes_256_key = "3383fa3b5b8c27ffe8b0a88080e2694e";

  // source data
  uint32_t source_data_len = 10;
  std::string default_source_data = "1704029500";
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

  fprintf(stderr, "encrypt size:%zu, and data is: ", encrypt_data.size());
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  std::string base64_encypt_data;
  ret = Base64Encode(encrypt_data, &base64_encypt_data);
  EXPECT_EQ(kOk, ret);
  fprintf(stderr, "base64_encypt_data:%s\n", base64_encypt_data.c_str());

  std::string encrypt_data_from_base64;
  ret = Base64Decode(base64_encypt_data, &encrypt_data_from_base64);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(encrypt_data, encrypt_data_from_base64);
  fprintf(stderr, "encrypt_data_from_base64 size:%zu, data:\n", encrypt_data_from_base64.size());
  for (uint32_t i = 0; i < encrypt_data_from_base64.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data_from_base64.data()[i]);
  }
  fprintf(stderr, "\n");

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data_from_base64, &decrypt_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_source_data, decrypt_data);
  fprintf(stderr, "decrypt: %s\n", decrypt_data.c_str());

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

/**
 * ECB
 */
TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_ECB);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * CFB
 */
TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * CFB8
 */
TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 10;
  std::string default_source_data = "abcd1efbh2";
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CFB8);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CFB8_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CFB8);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * CBC
 */
TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CBC);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CBC_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * OFB
 */
TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_OFB);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_OFB_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * GCM
 */
TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 10;
  std::string default_source_data = "abcd1efbh2";
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:size:%zu,  %s\n", default_source_data.size(), default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");
  std::string base64_encrypt_data;
  ret = Base64Encode(encrypt_data, &base64_encrypt_data);
  fprintf(stderr, "encrypt base64: %s\n", base64_encrypt_data.c_str());

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_source_data, decrypt_data);
  fprintf(stderr, "decrypt:size:%zu, %s\n", decrypt_data.size(), decrypt_data.c_str());

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
    fprintf(stderr, "default data:%s, decrypt_data:%s\n", default_source_data.c_str(), decrypt_data.c_str());
  }

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_GCM);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_GCM_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_GCM);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * CTR
 */
TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_10_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 10;
  std::string default_source_data = "abcd1efbh2";
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:size:%zu,  %s\n", default_source_data.size(), default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_source_data, decrypt_data);
  fprintf(stderr, "decrypt:size:%zu, %s\n", decrypt_data.size(), decrypt_data.c_str());

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_32_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_100_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_1024_Len) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_0_10240_Len) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  // source data
  for (uint32_t i = 0; i < 10240; ++i) {
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
    fprintf(stderr, "default data:%s, decrypt_data:%s\n", default_source_data.c_str(), decrypt_data.c_str());

    // std::string encrypt_data_base64;
    // ret = base::Base64Encode(encrypt_data, &encrypt_data_base64);
    // EXPECT_EQ(kOk, ret);
    // fprintf(stderr, "default data size:%zu, encrypt_data size:%zu, base64 encrypt size:%zu,
    // decrypt_data size:%zu\n", default_source_data.size(),
    //     encrypt_data.size(), encrypt_data_base64.size(), decrypt_data.size());
  }

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Exception_Empty_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:  %s\n", default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Exception_Null_Str) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_8096_BinStr_Len_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 8096;
  std::string default_source_data;
  Code ret = GetRandBinStr(source_data_len, &default_source_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Encrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Encrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Encrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Encrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Decrypt_10_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Decrypt_100_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Decrypt_1024_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_128key_Decrypt_8096_Len_Ten_Thousand) { /*{{{*/
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
  Cipher *aes_128 = new AESCipher(aes_128_key, AES_128_CTR);
  ret = aes_128->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_128->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_128->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_128;
  aes_128 = NULL;
} /*}}}*/

/**
 * NOTE:test integer
 */
TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_Integer) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 4;
  uint32_t test_num = 0xffeeaa11;
  fprintf(stderr, "source:%#x\n", test_num);
  std::string default_source_data(reinterpret_cast<char *>(&test_num), source_data_len);
  fprintf(stderr, "source: ");
  for (uint32_t i = 0; i < default_source_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)default_source_data.data()[i]);
  }
  fprintf(stderr, "\n");
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:size:%zu,  %s\n", default_source_data.size(), default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_source_data, decrypt_data);
  fprintf(stderr, "decrypt:size:%zu, %s\n", decrypt_data.size(), decrypt_data.c_str());

  fprintf(stderr, "decrypt: ");
  for (uint32_t i = 0; i < decrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)decrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "source:%#x, decrypt data:%#x\n", test_num,
          *(reinterpret_cast<uint32_t *>(const_cast<char *>((decrypt_data.data())))));
  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Exception_CheckIntegrity) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 4;
  uint32_t test_num = 0xffeeaa11;
  fprintf(stderr, "source:%#x\n", test_num);
  std::string default_source_data(reinterpret_cast<char *>(&test_num), source_data_len);
  fprintf(stderr, "source: ");
  for (uint32_t i = 0; i < default_source_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)default_source_data.data()[i]);
  }
  fprintf(stderr, "\n");
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:size:%zu,  %s\n", default_source_data.size(), default_source_data.c_str());

  // NOTE:htt, 调整密文的内容，验证CTR的不支持完整性校验的机制
  encrypt_data[0] = 'a';
  fprintf(stderr, "exception encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
  EXPECT_EQ(kOk, ret);

  // NOTE:htt, 因为AES CTR 模式并不支持对数据完整性的校验，所以当调整加密后数据内容
  // 通过CTR模式解密不能检查出数据内容的异常
  // 这里给出异常原始数据，以便确认异常密文解密前的内容
  std::string exception_source_data(4, '0');
  exception_source_data[0] = 0x4d;
  exception_source_data[1] = 0xaa;
  exception_source_data[2] = 0xee;
  exception_source_data[3] = 0xff;

  EXPECT_EQ(exception_source_data, decrypt_data);
  fprintf(stderr, "exception decrypt:size:%zu, %s\n", decrypt_data.size(), decrypt_data.c_str());
  fprintf(stderr, "exception decrypt: ");
  for (uint32_t i = 0; i < decrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)decrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "source:%#x, decrypt data:%#x\n", test_num,
          *(reinterpret_cast<uint32_t *>(const_cast<char *>((decrypt_data.data())))));
  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Encrypt_Integer_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 4;
  uint32_t test_num = 0xdfebaa99;
  std::string default_source_data(reinterpret_cast<char *>(&test_num), source_data_len);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
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
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Press_256key_Decrypt_Integer_Ten_Thousand) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";

  // source data
  uint32_t source_data_len = 10;
  uint32_t test_num = 0xdfebaa99;
  std::string default_source_data(reinterpret_cast<char *>(&test_num), source_data_len);
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  // Decrypt
  std::string decrypt_data;
  for (uint32_t i = 0; i < 10000; ++i) {
    ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
    EXPECT_EQ(kOk, ret);
  }
  EXPECT_EQ(default_source_data, decrypt_data);

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_10_Len_AssignIv) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
  std::string iv = "haveTryTwo123457";

  // source data
  uint32_t source_data_len = 10;
  std::string default_source_data = "abcd1efbh2";
  EXPECT_EQ(source_data_len, default_source_data.size());

  // Encrypt
  Cipher *aes_256 = new AESCipher(aes_256_key, iv, AES_256_CTR);
  Code ret = aes_256->Init();
  EXPECT_EQ(kOk, ret);

  std::string encrypt_data;
  ret = aes_256->Encrypt(default_source_data, &encrypt_data);
  EXPECT_EQ(kOk, ret);

  fprintf(stderr, "defuault data size:%zu, encrypt_data size:%zu\n", default_source_data.size(), encrypt_data.size());
  fprintf(stderr, "source:size:%zu,  %s\n", default_source_data.size(), default_source_data.c_str());

  fprintf(stderr, "encrypt: ");
  for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
    fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
  }
  fprintf(stderr, "\n");

  // Decrypt
  std::string decrypt_data;
  ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
  EXPECT_EQ(kOk, ret);
  EXPECT_EQ(default_source_data, decrypt_data);
  fprintf(stderr, "decrypt:size:%zu, %s\n", decrypt_data.size(), decrypt_data.c_str());

  delete aes_256;
  aes_256 = NULL;
} /*}}}*/

TEST(AESCipher, Test_CTR_Encrpyt_Decrypt_Normal_1_Len_Array_AssignIv) { /*{{{*/
  using namespace base;

  // key
  std::string aes_256_key = "ABCDEFGH--12827912--acefjmzl--00";
  std::string iv = "haveTryTwo123457";

  // source data
  std::string default_source_data_arr[] = {"bbbbbb", "a", "f", "2", "0", ""};
  AES_KEY_LEN_FLAG flags[] = {AES_128_ECB, AES_192_ECB, AES_256_ECB, AES_128_CBC,  AES_192_CBC,  AES_256_CBC,
                              AES_128_CFB, AES_192_CFB, AES_256_CFB, AES_128_CFB8, AES_192_CFB8, AES_256_CFB8,
                              AES_128_OFB, AES_192_OFB, AES_256_OFB, AES_128_GCM,  AES_192_GCM,  AES_256_GCM,
                              AES_128_CTR, AES_192_CTR, AES_256_CTR};

  for (int k = 0; k < static_cast<int>(sizeof(flags) / sizeof(flags[0])); ++k) {
    fprintf(stderr, "\nAES KEY flag:%d--------------------\n", flags[k]);
    char buf[kSmallBufLen] = {0};
    snprintf(buf, sizeof(buf) / sizeof(buf[0]) - 1, "%d", k);
    // Encrypt
    Cipher *aes_256 = new AESCipher(aes_256_key + buf, iv + buf, flags[k]);
    Code ret = aes_256->Init();
    EXPECT_EQ(kOk, ret);

    for (int i = 0; i < (int)(sizeof(default_source_data_arr) / sizeof(default_source_data_arr[0])); ++i) {
      std::string default_source_data = default_source_data_arr[i];
      std::string encrypt_data;
      ret = aes_256->Encrypt(default_source_data, &encrypt_data);
      EXPECT_EQ(kOk, ret);

      fprintf(stderr, "\ndefault source size:%zu, encrypt_data size:%zu\n", default_source_data.size(),
              encrypt_data.size());
      fprintf(stderr, "default source:size:%zu, value:%s\n", default_source_data.size(), default_source_data.c_str());

      fprintf(stderr, "encrypt: ");
      for (uint32_t i = 0; i < encrypt_data.size(); ++i) {
        fprintf(stderr, "%02x", (unsigned char)encrypt_data.data()[i]);
      }
      fprintf(stderr, "\n");

      // Decrypt
      std::string decrypt_data;
      ret = aes_256->Decrypt(encrypt_data, &decrypt_data);
      EXPECT_EQ(kOk, ret);
      EXPECT_EQ(default_source_data, decrypt_data);
      fprintf(stderr, "decrypt:size:%zu, value:%s\n", decrypt_data.size(), decrypt_data.c_str());
      fprintf(stderr, "default_source_data:%s\n", default_source_data.c_str());
    }

    delete aes_256;
    aes_256 = NULL;
  }
} /*}}}*/
