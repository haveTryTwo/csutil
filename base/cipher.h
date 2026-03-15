// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CIPHER_H_
#define BASE_CIPHER_H_

#include <stdint.h>

#include "base/status.h"

namespace base {

/**
 * Base class for cryptographic cipher operations
 *
 * This abstract class provides a unified interface for encryption, decryption,
 * signing, and verification operations. Subclasses should implement specific
 * cryptographic algorithms (e.g., AES, RSA).
 */
class Cipher {
 public:
  /**
   * Virtual destructor
   *
   * Ensures proper cleanup of derived class objects
   */
  virtual ~Cipher() {};

 public:
  /**
   * Initialize the cipher with necessary parameters
   *
   * This method should be called before using any encryption/decryption operations.
   * It sets up the cipher with required keys, algorithms, or other configuration.
   *
   * @return base::kOk if initialization succeeds, error code otherwise
   */
  virtual Code Init();

 public:
  /**
   * Encrypt data from a string source
   *
   * Encrypts the source data and stores the encrypted result in the output parameter.
   *
   * @param source_data The plaintext data to encrypt
   * @param encrpyt_data Output parameter to store the encrypted data
   * @return base::kOk if encryption succeeds, error code otherwise
   */
  virtual Code Encrypt(const std::string &source_data, std::string *encrpyt_data);

  /**
   * Encrypt data from a character buffer
   *
   * Encrypts the source data from a character buffer with specified length
   * and stores the encrypted result in the output parameter.
   *
   * @param source_data Pointer to the plaintext data buffer
   * @param len Length of the source data in bytes
   * @param encrpyt_data Output parameter to store the encrypted data
   * @return base::kOk if encryption succeeds, error code otherwise
   */
  virtual Code Encrypt(const char *source_data, uint32_t len, std::string *encrpyt_data);

  /**
   * Decrypt data from a string source
   *
   * Decrypts the encrypted data and stores the plaintext result in the output parameter.
   *
   * @param encrypt_data The encrypted data to decrypt
   * @param source_data Output parameter to store the decrypted plaintext data
   * @return base::kOk if decryption succeeds, error code otherwise
   */
  virtual Code Decrypt(const std::string &encrypt_data, std::string *source_data);

  /**
   * Decrypt data from a character buffer
   *
   * Decrypts the encrypted data from a character buffer with specified length
   * and stores the plaintext result in the output parameter.
   *
   * @param encrypt_data Pointer to the encrypted data buffer
   * @param len Length of the encrypted data in bytes
   * @param source_data Output parameter to store the decrypted plaintext data
   * @return base::kOk if decryption succeeds, error code otherwise
   */
  virtual Code Decrypt(const char *encrypt_data, uint32_t len, std::string *source_data);

  /**
   * Sign data to generate a digital signature
   *
   * Generates a digital signature for the source data using the cipher's private key.
   * The signature can be used later to verify the authenticity and integrity of the data.
   *
   * @param source_data The data to be signed
   * @param sign_data Output parameter to store the generated signature
   * @return base::kOk if signing succeeds, error code otherwise
   */
  virtual Code Sign(const std::string &source_data, std::string *sign_data);

  /**
   * Verify a digital signature
   *
   * Verifies the digital signature against the source data using the cipher's public key.
   *
   * @param soruce_data The original data that was signed
   * @param sign_data The digital signature to verify
   * @return base::kOk if verification succeeds (signature is valid), error code otherwise
   */
  virtual Code Verify(const std::string &soruce_data, const std::string &sign_data);

 protected:
  /**
   * Protected constructor
   *
   * Prevents direct instantiation of the base class. Only derived classes
   * can be instantiated, ensuring the class is used as an abstract interface.
   */
  Cipher() {}
};

}  // namespace base

#endif
