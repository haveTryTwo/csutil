// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CURL_HTTP_H_
#define BASE_CURL_HTTP_H_

#include <string>

#include <assert.h>
#include <curl/curl.h>

#include "base/status.h"

namespace base
{

/**
 * Note: see https://curl.haxx.se/libcurl/c/curl_global_init.html
 * 1. curl_global_init is not thread safe,
 * 2. curl_global_init  must be called at least once within a program before the program calls any other function in libcurl
 */
class CurlGlobalInit
{
    public:
        CurlGlobalInit()
        {
            CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
            assert(ret == 0);
        }

        ~CurlGlobalInit()
        {
            curl_global_cleanup();
        }
};

/**
 * Note: CurlHttp is a util that using curl library to get http result. But may pay attention to follows:
 *      1. this is not thread-safe function, as it has global variables
 */
class CurlHttp
{
    public:
        CurlHttp();
        ~CurlHttp();

        Code Init();
        Code Perform(const std::string &url, const std::string &post_params, std::string *result);
        Code Post(const std::string &url, const std::string &post_params, std::string *result);
        Code Get(const std::string &url, std::string *result);

        Code SetUserNameAndPwd(const std::string &user_name, const std::string &pwd);
        Code ClearUserNameAndPwd();

    private:
        CURL *curl_;
        bool is_keep_alive_;

        bool is_using_user_name_pwd_;
        std::string user_name_;
        std::string password_;

        static CurlGlobalInit curl_global_init_;
};

}

#endif
