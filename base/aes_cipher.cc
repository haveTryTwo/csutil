// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/aes_cipher.h"

#include <string.h>

#include "base/common.h"

namespace base {

AESCipher::AESCipher(const std::string &key, int key_len_flag)
    : key_(key), key_len_flag_(key_len_flag), evp_cipher_(NULL), is_init_(false) { /*{{{*/
  iv_ = "haveTryTwo123456";
} /*}}}*/

AESCipher::AESCipher(const std::string &key, const std::string &iv, int key_len_flag)
    : key_(key), iv_(iv), key_len_flag_(key_len_flag), evp_cipher_(NULL), is_init_(false) { /*{{{*/ } /*}}}*/

AESCipher::~AESCipher() { /*{{{*/ } /*}}}*/

Code AESCipher::Init() { /*{{{*/
  switch (key_len_flag_) {
    case AES_128_ECB:
      evp_cipher_ = EVP_aes_128_ecb();
      break;
    case AES_192_ECB:
      evp_cipher_ = EVP_aes_192_ecb();
      break;
    case AES_256_ECB:
      evp_cipher_ = EVP_aes_256_ecb();
      break;
    case AES_128_CBC:
      evp_cipher_ = EVP_aes_128_cbc();
      break;
    case AES_192_CBC:
      evp_cipher_ = EVP_aes_192_cbc();
      break;
    case AES_256_CBC:
      evp_cipher_ = EVP_aes_256_cbc();
      break;
    case AES_128_CFB:
      evp_cipher_ = EVP_aes_128_cfb();
      break;
    case AES_192_CFB:
      evp_cipher_ = EVP_aes_192_cfb();
      break;
    case AES_256_CFB:
      evp_cipher_ = EVP_aes_256_cfb();
      break;
    case AES_128_CFB8:
      evp_cipher_ = EVP_aes_128_cfb8();
      break;
    case AES_192_CFB8:
      evp_cipher_ = EVP_aes_192_cfb8();
      break;
    case AES_256_CFB8:
      evp_cipher_ = EVP_aes_256_cfb8();
      break;
    case AES_128_OFB:
      evp_cipher_ = EVP_aes_128_ofb();
      break;
    case AES_192_OFB:
      evp_cipher_ = EVP_aes_192_ofb();
      break;
    case AES_256_OFB:
      evp_cipher_ = EVP_aes_256_ofb();
      break;
    case AES_128_GCM:
      evp_cipher_ = EVP_aes_128_gcm();
      break;
    case AES_192_GCM:
      evp_cipher_ = EVP_aes_192_gcm();
      break;
    case AES_256_GCM:
      evp_cipher_ = EVP_aes_256_gcm();
      break;
    case AES_128_CTR:
      evp_cipher_ = EVP_aes_128_ctr();
      break;
    case AES_192_CTR:
      evp_cipher_ = EVP_aes_192_ctr();
      break;
    case AES_256_CTR:
      evp_cipher_ = EVP_aes_256_ctr();
      break;
    default:
      return kInvalidAESKeyLenFlag;
  }

  //    fprintf(stderr, "AES_BLOCK_SIZE:%d\n", AES_BLOCK_SIZE);
  is_init_ = true;
  return kOk;
} /*}}}*/

Code AESCipher::Encrypt(const std::string &source_data, std::string *encrpyt_data) { /*{{{*/
  if (encrpyt_data == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  return Encrypt(source_data.data(), source_data.size(), encrpyt_data);
} /*}}}*/

Code AESCipher::Encrypt(const char *source_data, uint32_t len, std::string *encrpyt_data) { /*{{{*/
  if (encrpyt_data == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  int out_len = 0;
  int new_len = 0;
  size_t max_out_size = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
  Code ret = kOk;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) return kEVPCIPHERCTXNewFailed;

  int r = 0;
  if (evp_cipher_ == EVP_aes_128_gcm() || evp_cipher_ == EVP_aes_192_gcm() || evp_cipher_ == EVP_aes_256_gcm()) {
    r = EVP_EncryptInit_ex(ctx, evp_cipher_, NULL, NULL, NULL);
    if (r != 1) {
      ret = kEVPEncryptInitExFailed;
      goto err;
    }

    r = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_.size(), NULL);
    if (r != 1) {
      ret = kEVPCtrlSetIvFailed;
      goto err;
    }

    r = EVP_EncryptInit_ex(ctx, NULL, NULL, (const unsigned char *)key_.c_str(), (const unsigned char *)iv_.c_str());
    if (r != 1) {
      ret = kEVPEncryptInitExFailed;
      goto err;
    }
  } else {
    r = EVP_EncryptInit_ex(ctx, evp_cipher_, NULL, (const unsigned char *)key_.c_str(),
                           (const unsigned char *)iv_.c_str());
    if (r != 1) {
      ret = kEVPEncryptInitExFailed;
      goto err;
    }
  }

  encrpyt_data->resize(max_out_size);

  r = EVP_EncryptUpdate(ctx, (unsigned char *)const_cast<char *>(encrpyt_data->data()), &out_len,
                        (const unsigned char *)source_data, len);
  if (r != 1) {
    ret = kEVPEncryptUpdateFailed;
    goto err;
  }
  new_len = out_len;

  r = EVP_EncryptFinal_ex(ctx, (unsigned char *)const_cast<char *>(encrpyt_data->data()) + out_len, &out_len);
  if (r != 1) {
    ret = kEVPEncryptFinalExFailed;
    goto err;
  }
  new_len += out_len;

  encrpyt_data->resize(new_len);

  if (evp_cipher_ == EVP_aes_128_gcm() || evp_cipher_ == EVP_aes_192_gcm() || evp_cipher_ == EVP_aes_256_gcm()) {
    char tag[kSmallBufLen] = {0};
    r = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, kAESTagLen, tag);
    if (r != 1) {
      ret = kEVPCtrlTagFailed;
      goto err;
    }
    encrpyt_data->append(tag, kAESTagLen);
  }

  EVP_CIPHER_CTX_free(ctx);

  return kOk;

err:
  EVP_CIPHER_CTX_free(ctx);
  return ret;
} /*}}}*/

Code AESCipher::Decrypt(const std::string &encrypt_data, std::string *source_data) { /*{{{*/
  if (source_data == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  return Decrypt(encrypt_data.data(), encrypt_data.size(), source_data);
} /*}}}*/

Code AESCipher::Decrypt(const char *encrypt_data, uint32_t len, std::string *source_data) { /*{{{*/
  if (source_data == NULL) return kInvalidParam;
  if (!is_init_) return kNotInit;

  Code ret = kOk;
  int out_len = 0;
  int new_len = 0;
  size_t max_out_size = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
  size_t encrypt_size = len;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) return kEVPCIPHERCTXNewFailed;

  int r = 0;
  if (evp_cipher_ == EVP_aes_128_gcm() || evp_cipher_ == EVP_aes_192_gcm() || evp_cipher_ == EVP_aes_256_gcm()) {
    r = EVP_DecryptInit_ex(ctx, evp_cipher_, NULL, NULL, NULL);
    if (r != 1) {
      ret = kEVPEncryptInitExFailed;
      goto err;
    }

    r = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_.size(), NULL);
    if (r != 1) {
      ret = kEVPCtrlSetIvFailed;
      goto err;
    }

    r = EVP_DecryptInit_ex(ctx, NULL, NULL, (const unsigned char *)key_.c_str(), (const unsigned char *)iv_.c_str());
    if (r != 1) {
      ret = kEVPDecryptInitExFailed;
      goto err;
    }
  } else {
    r = EVP_DecryptInit_ex(ctx, evp_cipher_, NULL, (const unsigned char *)key_.c_str(),
                           (const unsigned char *)iv_.c_str());
    if (r != 1) {
      ret = kEVPDecryptInitExFailed;
      goto err;
    }
  }

  if (evp_cipher_ == EVP_aes_128_gcm() || evp_cipher_ == EVP_aes_192_gcm() || evp_cipher_ == EVP_aes_256_gcm()) {
    if (len < kAESTagLen) {
      ret = kInvalidEncryptDataSize;
      goto err;
    }

    char tag[kSmallBufLen] = {0};
    memcpy(tag, encrypt_data + len - kAESTagLen, kAESTagLen);
    r = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, kAESTagLen, (void *)tag);
    if (r != 1) {
      ret = kEVPCtrlTagFailed;
      goto err;
    }
    encrypt_size = len - kAESTagLen;
  }

  source_data->resize(max_out_size);

  r = EVP_DecryptUpdate(ctx, (unsigned char *)const_cast<char *>(source_data->data()), &out_len,
                        (const unsigned char *)encrypt_data, encrypt_size);
  if (r != 1) {
    ret = kEVPDecryptUpdateFailed;
    goto err;
  }
  new_len = out_len;

  r = EVP_DecryptFinal_ex(ctx, (unsigned char *)const_cast<char *>(source_data->data()) + out_len, &out_len);
  if (r != 1) {
    ret = kEVPDecryptFinalExFailed;
    goto err;
  }
  new_len += out_len;
  source_data->resize(new_len);

  EVP_CIPHER_CTX_free(ctx);

  return kOk;

err:
  EVP_CIPHER_CTX_free(ctx);
  return ret;
} /*}}}*/

}  // namespace base
