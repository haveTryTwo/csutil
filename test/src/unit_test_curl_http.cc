// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "base/common.h"
#include "base/curl_http.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(CurlHttp, Test_Normal_Post) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://www.baidu.com";
  std::string post_params;
  std::string result;

  r = curl_http.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_Get) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://www.baidu.com";
  std::string result;

  r = curl_http.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_Get_Localhost) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "localhost:9123";

  while (true) {
    std::string result;
    r = curl_http.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
    usleep(1000);
    break;
  }
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Post_Bulk) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/_bulk?pretty";
  std::string post_params =
      "{ \"delete\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"create\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"EEEEE\", \"country\" : \"China\", \"age\" : 119}\n"
      "{ \"index\":  { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 12 }}\n"
      "{ \"name\" : \"FFFFF\", \"country\" : \"US\", \"age\" : 118}\n"
      "{ \"update\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 13, "
      "\"_retry_on_conflict\" : 3} }\n"
      "{ \"doc\" : {\"title\" : \"Very good and yes\"}, \"doc_as_upsert\": true}\n";
  std::string result;

  r = curl_http.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Post_Search) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_search?pretty";
  std::string post_params = "{}";
  std::string result;

  r = curl_http.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Get_ID) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_doc/11?pretty";
  std::string result;

  r = curl_http.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Conflict_ID) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url =
      "http://localhost:9200/tests_3/_doc/12?version=10&version_type=external_gt&pretty";
  std::string large_str = "aaa";
  for (int i = 0; i < 2000; ++i) {
    large_str.append(1, 'A' + i % 26);
  }
  std::string post_params =
      "{ \"name\":\"" + large_str + "EEEEE\",  \"country\":\"China\", \"age\":119 }";
  std::string result;

  for (int i = 0; i < 5; ++i) {
    r = curl_http.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }
  }
  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_Get_Localhost_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = curl_http.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "localhost:9123";

  while (true) {
    std::string result;
    r = curl_http.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
    usleep(1000);
    break;
  }
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Post_Bulk_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = curl_http.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/_bulk?pretty";
  std::string post_params =
      "{ \"delete\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"create\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"EEEEE\", \"country\" : \"China\", \"age\" : 119}\n"
      "{ \"index\":  { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 12 }}\n"
      "{ \"name\" : \"FFFFF\", \"country\" : \"US\", \"age\" : 118}\n"
      "{ \"update\": { \"_index\": \"tests_3\", \"_type\": \"_doc\", \"_id\": 13, "
      "\"_retry_on_conflict\" : 3} }\n"
      "{ \"doc\" : {\"title\" : \"Very good and yes\"}, \"doc_as_upsert\": true}\n";
  std::string result;

  r = curl_http.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Post_Search_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = curl_http.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_search?pretty";
  std::string post_params = "{}";
  std::string result;

  r = curl_http.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Get_ID_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = curl_http.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_doc/11?pretty";
  std::string result;

  r = curl_http.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(CurlHttp, Test_Normal_ES_Conflict_ID_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  CurlHttp curl_http;
  Code r = curl_http.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = curl_http.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url =
      "http://localhost:9200/tests_3/_doc/12?version=10&version_type=external_gt&pretty";
  std::string large_str = "aaa";
  for (int i = 0; i < 2000; ++i) {
    large_str.append(1, 'A' + i % 26);
  }
  std::string post_params =
      "{ \"name\":\"" + large_str + "EEEEE\",  \"country\":\"China\", \"age\":119 }";
  std::string result;

  for (int i = 0; i < 5; ++i) {
    r = curl_http.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }
  }
  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/
