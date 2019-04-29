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
        Code Perform(const std::string &url, const std::string &content, std::string *result);
        Code Post(const std::string &url, const std::string &content, std::string *result);
        Code Get(const std::string &url, std::string *result);

    private:
        CURL *curl_;
        bool is_keep_alive_;

        static CurlGlobalInit curl_global_init_;
};

}

#endif
