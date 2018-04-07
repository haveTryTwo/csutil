// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/aes_cipher.h"

namespace base
{

AESCipher::AESCipher(std::string key, int key_len_flag) : key_(key),
    key_len_flag_(key_len_flag), evp_cipher_(NULL), is_init_(false)
{/*{{{*/
    iv_ = "haveTryTwo123456";
}/*}}}*/

AESCipher::~AESCipher()
{/*{{{*/
}/*}}}*/


Code AESCipher::Init()
{/*{{{*/
    switch(key_len_flag_)
    {
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
        case AES_128_OFB:
            evp_cipher_ = EVP_aes_128_ofb();
            break;
        case AES_192_OFB:
            evp_cipher_ = EVP_aes_192_ofb();
            break;
        case AES_256_OFB:
            evp_cipher_ = EVP_aes_256_ofb();
            break;
        default:
            return kInvalidAESKeyLenFlag;
    }

    fprintf(stderr, "AES_BLOCK_SIZE:%d\n", AES_BLOCK_SIZE);
    is_init_ = true;
    return kOk;
}/*}}}*/

Code AESCipher::Encrypt(const std::string &source_data, std::string *encrpyt_data)
{/*{{{*/
    if (encrpyt_data == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    int out_len = 0;
    int new_len = 0;
    size_t max_out_size = (source_data.size()/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    Code ret = kOk;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    int r = EVP_EncryptInit_ex(&ctx, evp_cipher_, NULL, (const unsigned char*)key_.c_str(), (const unsigned char*)iv_.c_str());
    if (r != 1) 
    {
        ret = kEVPEncryptInitExFailed;
        goto err;
    }

    encrpyt_data->resize(max_out_size);

    r = EVP_EncryptUpdate(&ctx, (unsigned char*)const_cast<char*>(encrpyt_data->data()), &out_len,
        (const unsigned char*)source_data.data(), source_data.size());
    if (r != 1)
    {
        ret = kEVPEncryptUpdateFailed;
        goto err;
    }
    new_len = out_len;

    r = EVP_EncryptFinal_ex(&ctx, (unsigned char*)const_cast<char*>(encrpyt_data->data())+out_len, &out_len);
    if (r != 1)
    {
        ret = kEVPEncryptFinalExFailed;
        goto err;
    }
    new_len += out_len;

    encrpyt_data->resize(new_len);
    EVP_CIPHER_CTX_cleanup(&ctx);

    return kOk;

err:
    EVP_CIPHER_CTX_cleanup(&ctx);
    return ret;
}/*}}}*/

Code AESCipher::Decrypt(const std::string &encrypt_data, std::string *source_data)
{/*{{{*/
    if (source_data == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    Code ret = kOk;
    int out_len = 0;
    int new_len = 0;
    size_t max_out_size = (encrypt_data.size()/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    int r = EVP_DecryptInit_ex(&ctx, evp_cipher_, NULL, (const unsigned char*)key_.c_str(), (const unsigned char*)iv_.c_str());
    if (r != 1) 
    {
        ret = kEVPDecryptInitExFailed;
        goto err;
    }

    source_data->resize(max_out_size);

    r = EVP_DecryptUpdate(&ctx, (unsigned char*)const_cast<char*>(source_data->data()), &out_len,
        (const unsigned char*)encrypt_data.data(), encrypt_data.size());
    if (r != 1)
    {
        ret = kEVPDecryptUpdateFailed;
        goto err;
    }
    new_len = out_len;

    r = EVP_DecryptFinal_ex(&ctx, (unsigned char*)const_cast<char*>(source_data->data())+out_len, &out_len);
    if (r != 1)
    {
        ret = kEVPDecryptFinalExFailed;
        goto err;
    }
    new_len += out_len;
    source_data->resize(new_len);

    EVP_CIPHER_CTX_cleanup(&ctx);

    return kOk;

err:
    EVP_CIPHER_CTX_cleanup(&ctx);
    return ret;
}/*}}}*/

}
