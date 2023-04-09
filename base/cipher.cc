// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/cipher.h"

namespace base {

Code Cipher::Init() { return kOtherFailed; }

Code Cipher::Encrypt(const std::string &source_data, std::string *encrpyt_data) {
  return kOtherFailed;
}

Code Cipher::Encrypt(const char *source_data, uint32_t len, std::string *encrpyt_data) {
  return kOtherFailed;
}

Code Cipher::Decrypt(const std::string &encrypt_data, std::string *source_data) {
  return kOtherFailed;
}

Code Cipher::Decrypt(const char *encrypt_data, uint32_t len, std::string *source_data) {
  return kOtherFailed;
}

Code Cipher::Sign(const std::string &source_data, std::string *sign_data) { return kOtherFailed; }

Code Cipher::Verify(const std::string &soruce_data, const std::string &sign_data) {
  return kOtherFailed;
}

}  // namespace base
