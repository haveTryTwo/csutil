// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "base/curl_http.h"

namespace base
{

CurlGlobalInit   CurlHttp::curl_global_init_;

size_t RespHttpData( char *ptr, size_t size, size_t nmemb, void *userdata)
{/*{{{*/
    if (ptr == NULL) return 0;
    uint32_t real_size = size * nmemb;

    std::string *result = (std::string*)(userdata);

    result->append(ptr, real_size);

    return real_size;
}/*}}}*/

CurlHttp::CurlHttp() : curl_(NULL), is_keep_alive_(true)
{
}

CurlHttp::~CurlHttp()
{/*{{{*/
    if (curl_ != NULL)
    {
        curl_easy_cleanup(curl_);
        curl_ = NULL;
    }
}/*}}}*/

Code CurlHttp::Init()
{/*{{{*/
    curl_ = curl_easy_init();
    if (curl_ == NULL) return kCurlEasyInitFailed;

    return kOk;
}/*}}}*/

Code CurlHttp::Perform(const std::string &url, const std::string &content, std::string *result)
{/*{{{*/
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, content.c_str());
    curl_slist* header = NULL;
    if (is_keep_alive_)
    {
        header = curl_slist_append(header, "Connection: keep-alive");
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);
    }

    curl_easy_setopt(curl_, CURLOPT_POST, 1);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, RespHttpData);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void*)result);

    CURLcode ret = curl_easy_perform(curl_);
    if(is_keep_alive_)
        curl_slist_free_all(header);

    if (ret != 0)
    {
        curl_easy_reset(curl_);
        return kCurlEasyPerformFailed;
    }

    return kOk;
}/*}}}*/

Code CurlHttp::Post(const std::string &url, const std::string &content, std::string *result)
{/*{{{*/
    return Perform(url, content, result);
}/*}}}*/

Code CurlHttp::Get(const std::string &url, std::string *result)
{/*{{{*/
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl_, CURLOPT_POST, 0);
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, RespHttpData);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void*)result);
    curl_slist* header = NULL;
    if (is_keep_alive_)
    {
        header = curl_slist_append(header, "Connection: keep-alive");
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);
    }

    CURLcode ret = curl_easy_perform(curl_);
    if(is_keep_alive_)
        curl_slist_free_all(header);

    if (ret != 0)
    {
        curl_easy_reset(curl_);
        return kCurlEasyPerformFailed;
    }

    return kOk;
}/*}}}*/

}

#ifdef _CURL_HTTP_MAIN_TEST_

#include <stdio.h>
int main(int argc, char *argv[])
{
    using namespace base;

    CurlHttp curl_http;
    curl_http.Init();

//    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
    std::string url = "http://www.baidu.com";
    std::string content;
    std::string result;

    Code r = curl_http.Perform(url, content, &result);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get result of url:%s, content:%s, ret:%d\n",
                url.c_str(), content.c_str(), r);
        return r;
    }

    fprintf(stderr, "%s\n", result.c_str());

    return 0;
}


#endif
