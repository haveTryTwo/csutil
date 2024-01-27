// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/curl_http.h"

#include <stdint.h>

namespace base {

CurlGlobalInit CurlHttp::curl_global_init_;

size_t RespHttpData(char *ptr, size_t size, size_t nmemb, void *userdata) { /*{{{*/
  if (ptr == NULL) return 0;
  uint32_t real_size = size * nmemb;

  std::string *result = (std::string *)(userdata);

  result->append(ptr, real_size);

  return real_size;
} /*}}}*/

CurlHttp::CurlHttp() : curl_(NULL), is_keep_alive_(true), is_using_user_name_pwd_(false) {}

CurlHttp::~CurlHttp() { /*{{{*/
  if (curl_ != NULL) {
    curl_easy_cleanup(curl_);
    curl_ = NULL;
  }
} /*}}}*/

Code CurlHttp::Init() { /*{{{*/
  curl_ = curl_easy_init();
  if (curl_ == NULL) return kCurlEasyInitFailed;

  return kOk;
} /*}}}*/

Code CurlHttp::Perform(const std::string &url, const std::string &post_params,
                       std::string *result) { /*{{{*/
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  // curl_easy_setopt(curl_, CURLOPT_FAILONERROR, 1); // http.retcode >= 400 while return error
  curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_params.c_str());
  curl_slist *header = NULL;
  if (is_keep_alive_) {
    header = curl_slist_append(header, "Connection: keep-alive");
    curl_slist_append(header, "Content-Type: application/json");
    curl_slist_append(header, "Expect: ");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);
  }

  curl_easy_setopt(curl_, CURLOPT_POST, 1);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, RespHttpData);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)result);
  curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, 5000L);

  // https://curl.se/libcurl/c/CURLOPT_USERNAME.html
  if (is_using_user_name_pwd_) {
    curl_easy_setopt(curl_, CURLOPT_USERNAME, user_name_.c_str());
    curl_easy_setopt(curl_, CURLOPT_PASSWORD, password_.c_str());
  }

  CURLcode ret = curl_easy_perform(curl_);
  if (is_keep_alive_) curl_slist_free_all(header);

  if (ret != 0) {
    curl_easy_reset(curl_);
    fprintf(stderr, "curl return code:%d\n", ret);
    return kCurlEasyPerformFailed;
  }

  return kOk;
} /*}}}*/

Code CurlHttp::Post(const std::string &url, const std::string &post_params,
                    std::string *result) { /*{{{*/
  return Perform(url, post_params, result);
} /*}}}*/

Code CurlHttp::Get(const std::string &url, std::string *result) { /*{{{*/
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

  curl_easy_setopt(curl_, CURLOPT_POST, 0);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, RespHttpData);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)result);
  curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, 5000L);
  curl_slist *header = NULL;
  if (is_keep_alive_) {
    header = curl_slist_append(header, "Connection: keep-alive");
    curl_slist_append(header, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);
  }

  // https://curl.se/libcurl/c/CURLOPT_USERNAME.html
  if (is_using_user_name_pwd_) {
    curl_easy_setopt(curl_, CURLOPT_USERNAME, user_name_.c_str());
    curl_easy_setopt(curl_, CURLOPT_PASSWORD, password_.c_str());
  }

  CURLcode ret = curl_easy_perform(curl_);
  if (is_keep_alive_) curl_slist_free_all(header);

  if (ret != 0) {
    fprintf(stderr, "curl return code:%d\n", ret);
    curl_easy_reset(curl_);
    return kCurlEasyPerformFailed;
  }

  return kOk;
} /*}}}*/

Code CurlHttp::SetUserNameAndPwd(const std::string &user_name, const std::string &pwd) { /*{{{*/
  user_name_ = user_name;
  password_ = pwd;

  is_using_user_name_pwd_ = true;

  return kOk;
} /*}}}*/

Code CurlHttp::ClearUserNameAndPwd() { /*{{{*/
  user_name_.clear();
  password_.clear();
  is_using_user_name_pwd_ = false;

  return kOk;
} /*}}}*/

}  // namespace base

#ifdef _CURL_HTTP_MAIN_TEST_

#include <stdio.h>
int main(int argc, char *argv[]) {
  using namespace base;

  CurlHttp curl_http;
  curl_http.Init();

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://www.baidu.com";
  std::string post_params;
  std::string result;

  Code r = curl_http.Perform(url, post_params, &result);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
    return r;
  }

  fprintf(stderr, "%s\n", result.c_str());

  return 0;
}

#endif
