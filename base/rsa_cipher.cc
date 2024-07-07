// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/rsa_cipher.h"

#include <assert.h>
#include <stdint.h>

#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace base {

static const int kShaDigestLength = 20;
static const std::string kSeed = "Try new seed for entropy!";

/**
 * Note: When Using RSA_public_encrypt() :
 * flen must be less than RSA_size(rsa) - 11 for the PKCS #1 v1.5 based padding modes, less than
 * RSA_size(rsa) - 41 for RSA_PKCS1_OAEP_PADDING and exactly RSA_size(rsa) for RSA_NO_PADDING. The
 * random number generator must be seeded prior to calling RSA_public_encrypt()
 */
static const int kFlenDiff = 12;

RsaCipher::RsaCipher(const std::string &key_path) : key_path_(key_path) { /*{{{*/
  key_ = NULL;
  RAND_seed(kSeed.data(), kSeed.size());
} /*}}}*/

RsaCipher::~RsaCipher() { /*{{{*/
  if (key_ != NULL) {
    RSA_free(key_);
    key_ = NULL;
  }
} /*}}}*/

Code RsaCipher::Init() { /*{{{*/
  Code ret = GetKey();
  return ret;
} /*}}}*/

// Note: Public Key is used for encrypt
Code RsaCipher::Encrypt(const std::string &source_data, std::string *encrypt_data) { /*{{{*/
  if (encrypt_data == NULL) return kInvalidParam;

  int key_size = RSA_size(key_);
  assert(key_size > 0);
  int block_size = key_size - kFlenDiff;
  assert(block_size > 0);
  int block_num = source_data.size() / block_size;
  int left_size = source_data.size() % block_size;

  unsigned char *buf = NULL;
  int buf_len = 0;
  if (left_size == 0) {
    buf = new unsigned char[block_num * key_size];
  } else {
    buf = new unsigned char[(block_num + 1) * key_size];
  }

  for (int block_index = 0; block_index < block_num; ++block_index) {
    int ret = RSA_public_encrypt(block_size, (unsigned char *)source_data.data() + block_index * block_size,
                                 buf + buf_len, key_, RSA_PKCS1_PADDING);
    if (ret < 0) {
      delete[] buf;
      return kEncryptFailed;
    }
    buf_len += ret;
  }

  if (left_size != 0) {
    int ret = RSA_public_encrypt(left_size, (unsigned char *)source_data.data() + block_num * block_size, buf + buf_len,
                                 key_, RSA_PKCS1_PADDING);
    if (ret < 0) {
      delete[] buf;
      return kEncryptFailed;
    }
    buf_len += ret;
  }

  encrypt_data->assign((char *)buf, buf_len);
  delete[] buf;

  return kOk;
} /*}}}*/

Code RsaCipher::Decrypt(const std::string &encrypt_data, std::string *source_data) { /*{{{*/
  if (source_data == NULL) return kInvalidParam;
  int key_size = RSA_size(key_);
  assert(key_size > 0);
  int block_size = key_size - kFlenDiff;
  int block_num = encrypt_data.size() / key_size;
  if (encrypt_data.size() % key_size != 0) {
    return kInvalidEncryptDataSize;
  }

  char *buf = new char[block_size * block_num];
  int buf_len = 0;

  for (int block_index = 0; block_index < block_num; ++block_index) {
    int ret = RSA_private_decrypt(key_size, (unsigned char *)encrypt_data.data() + block_index * key_size,
                                  (unsigned char *)buf + buf_len, key_, RSA_PKCS1_PADDING);
    if (ret < 1) {
      delete[] buf;
      return kDecryptFailed;
    }
    buf_len += ret;
  }

  source_data->assign(buf, buf_len);
  delete[] buf;

  return kOk;
} /*}}}*/

Code RsaCipher::Sign(const std::string &source_data, std::string *sign_data) { /*{{{*/
  if (sign_data == NULL) return kInvalidParam;

  int key_size = RSA_size(key_);
  unsigned char *sha_digest = new unsigned char[kShaDigestLength];
  unsigned char *tmp_buf = sha_digest;
  Code ret = kOk;
  sha_digest = SHA1((unsigned char *)source_data.data(), source_data.size(), sha_digest);
  if (sha_digest == NULL) {
    delete[] tmp_buf;
    tmp_buf = NULL;
    return kSHA1Failed;
  }

  unsigned char *buf = new unsigned char[key_size + 1];
  unsigned int buf_len = 0;
  if (RSA_sign(NID_sha1, sha_digest, kShaDigestLength, buf, &buf_len, key_) != 1) {
    ret = kRSASignFailed;
    goto err;
  }
  if (buf_len != (uint32_t)key_size) {
    ret = kInvalidSignSize;
    goto err;
  }

  sign_data->assign((char *)buf, buf_len);

err:
  delete[] buf;
  buf = NULL;
  delete[] tmp_buf;
  tmp_buf = NULL;

  return ret;
} /*}}}*/

Code RsaCipher::Verify(const std::string &source_data, const std::string &sign_data) { /*{{{*/
  int key_size = RSA_size(key_);
  unsigned char *sha_digest = new unsigned char[kShaDigestLength];
  unsigned char *tmp_buf = sha_digest;

  Code ret = kOk;
  if (sign_data.size() != (uint32_t)key_size) {
    ret = kInvalidSignSize;
    goto exception;
  }

  sha_digest = SHA1((unsigned char *)source_data.data(), source_data.size(), sha_digest);
  if (sha_digest == NULL) {
    ret = kSHA1Failed;
    goto exception;
  }

  if (RSA_verify(NID_sha1, sha_digest, kShaDigestLength, (unsigned char *)sign_data.data(), sign_data.size(), key_) !=
      1) {
    ret = kRSAVerifyFailed;
    goto exception;
  }

exception:
  delete[] tmp_buf;
  tmp_buf = NULL;

  return ret;
} /*}}}*/

PubRsaCipher::PubRsaCipher(const std::string &key_path) : RsaCipher(key_path) { /*{{{*/
} /*}}}*/

Code PubRsaCipher::GetKey() { /*{{{*/
  Code ret = GetPubKey();
  return ret;
} /*}}}*/

Code PubRsaCipher::GetPubKey() { /*{{{*/
  FILE *fp = fopen(key_path_.c_str(), "r");
  if (fp == NULL) return kOpenFileFailed;

  // Note: PEM_read_RSA_PUBKEY may leak memory
  RSA *r = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
  if (r == NULL) return kReadFailed;
  key_ = r;

  fclose(fp);
  fp = NULL;

  return kOk;
} /*}}}*/

PriRsaCipher::PriRsaCipher(const std::string &key_path) : RsaCipher(key_path) { /*{{{*/
} /*}}}*/

Code PriRsaCipher::GetKey() { /*{{{*/
  Code ret = GetPriKey();
  return ret;
} /*}}}*/

Code PriRsaCipher::GetPriKey() { /*{{{*/
  FILE *fp = fopen(key_path_.c_str(), "r");
  if (fp == NULL) return kOpenFileFailed;
  RSA *r = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
  if (r == NULL) return kReadFailed;
  key_ = r;

  fclose(fp);
  fp = NULL;

  return kOk;
} /*}}}*/
}  // namespace base
