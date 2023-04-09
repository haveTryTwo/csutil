// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_RSA_CIPHER_H_
#define BASE_RSA_CIPHER_H_

#include <string>

#include <openssl/rsa.h>

#include "base/cipher.h"
#include "base/status.h"

namespace base {

class RsaCipher : public Cipher {
 public:
  RsaCipher(const std::string &key_path);
  virtual ~RsaCipher();

 public:
  virtual Code Init();

 public:
  virtual Code Encrypt(const std::string &source_data, std::string *encrpyt_data);
  virtual Code Decrypt(const std::string &encrypt_data, std::string *source_data);

  virtual Code Sign(const std::string &source_data, std::string *sign_data);
  virtual Code Verify(const std::string &source_data, const std::string &sign_data);

  virtual Code GetKey() = 0;

 protected:
  std::string key_path_;
  RSA *key_;
};

class PubRsaCipher : public RsaCipher {
 public:
  PubRsaCipher(const std::string &key_path);

 public:
  virtual Code GetKey();

 private:
  Code GetPubKey();
};

class PriRsaCipher : public RsaCipher {
 public:
  PriRsaCipher(const std::string &key_path);

 public:
  virtual Code GetKey();

 private:
  Code GetPriKey();
};

}  // namespace base

#endif
