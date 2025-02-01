// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <stdint.h>
#include <stdio.h>

#include "base/hash.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Murmur32, Test_Normal) { /*{{{*/
  using namespace base;

  std::string key = "10.12.13.14:2010:100";
  uint32_t seed = 0x12345678;
  uint32_t expect_num = 537653845;
  uint32_t num = Murmur32(key, seed);
  EXPECT_EQ(expect_num, num);
} /*}}}*/

TEST(Murmur32, Test_Normal_Empty_Str) { /*{{{*/
  using namespace base;

  std::string key = "";
  uint32_t seed = 0x12345678;
  uint32_t expect_num = 3816608188;
  uint32_t num = Murmur32(key, seed);
  EXPECT_EQ(expect_num, num);
} /*}}}*/

TEST(GetEqualOrUpperBound, Test_Normal) { /*{{{*/
  using namespace base;

  std::map<int, int> test_map;
  test_map.insert(std::pair<int, int>(10, 10));
  test_map.insert(std::pair<int, int>(20, 20));
  test_map.insert(std::pair<int, int>(30, 30));
  test_map.insert(std::pair<int, int>(40, 40));

  std::map<int, int>::const_iterator it;

  int num = 35;
  int upper_bound = 40;
  it = GetEqualOrUpperBound(test_map, num);
  EXPECT_NEQ(test_map.end(), it);
  EXPECT_EQ(upper_bound, it->first);

  num = 45;
  it = GetEqualOrUpperBound(test_map, num);
  EXPECT_EQ(test_map.end(), it);
} /*}}}*/

TEST(GetEqualOrUpperBound, Test_Equal) { /*{{{*/
  using namespace base;

  std::map<int, int> test_map;
  test_map.insert(std::pair<int, int>(10, 10));
  test_map.insert(std::pair<int, int>(20, 20));
  test_map.insert(std::pair<int, int>(30, 30));
  test_map.insert(std::pair<int, int>(40, 40));

  std::map<int, int>::const_iterator it;

  int num = 20;
  int upper_bound = 20;
  it = GetEqualOrUpperBound(test_map, num);
  EXPECT_NEQ(test_map.end(), it);
  EXPECT_EQ(upper_bound, it->first);

  num = 40;
  upper_bound = 40;
  it = GetEqualOrUpperBound(test_map, num);
  EXPECT_NEQ(test_map.end(), it);
  EXPECT_EQ(upper_bound, it->first);
} /*}}}*/

TEST(GetEqualOrUpperBound, Test_To_LowerBound) { /*{{{*/
  using namespace base;

  std::map<int, int> test_map;
  test_map.insert(std::pair<int, int>(10, 10));
  test_map.insert(std::pair<int, int>(20, 20));
  test_map.insert(std::pair<int, int>(30, 30));
  test_map.insert(std::pair<int, int>(40, 40));

  std::map<int, int>::const_iterator equal_or_upper_it;
  std::map<int, int>::const_iterator lower_bound_it;

  for (int i = 0; i < 50; ++i) {
    equal_or_upper_it = GetEqualOrUpperBound(test_map, i);
    lower_bound_it = test_map.lower_bound(i);

    EXPECT_EQ(equal_or_upper_it, lower_bound_it);
  }
} /*}}}*/

TEST(BKDRHash, Test_Normal) { /*{{{*/
  using namespace base;

  std::string key = "Test goood one";
  uint32_t num = BKDRHash(key.c_str());
  fprintf(stderr, "BKDRHash: %u\n", num);
} /*}}}*/

TEST(BKDRHash, Test_Press_Balance) { /*{{{*/
  using namespace base;

  std::string key;
  uint32_t key_len = 32;
  std::map<uint32_t, uint32_t> hash_info;
  for (uint32_t i = 0; i < 1000 * 1000; ++i) {
    Code ret = GetRandStr(key_len, &key);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(key_len, key.size());

    uint32_t num = BKDRHash(key.c_str());
    hash_info[num % 10]++;
  }

  std::map<uint32_t, uint32_t>::iterator it;
  fprintf(stderr, "BKDRHash: mod 10\n");
  for (it = hash_info.begin(); it != hash_info.end(); ++it) {
    fprintf(stderr, "   %u:%u\n", it->first, it->second);
  }
  fprintf(stderr, "\n");
} /*}}}*/

TEST(HashString, Test_Normal) { /*{{{*/
  using namespace base;

  std::string key = "Test goood one";
  uint32_t num = HashString(key.c_str());
  fprintf(stderr, "HashString: %u\n", num);
} /*}}}*/

TEST(HashString, Test_Press_Balance) { /*{{{*/
  using namespace base;

  std::string key;
  uint32_t key_len = 32;
  std::map<uint32_t, uint32_t> hash_info;
  for (uint32_t i = 0; i < 1000 * 1000; ++i) {
    Code ret = GetRandStr(key_len, &key);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(key_len, key.size());

    uint32_t num = HashString(key.c_str());
    hash_info[num % 10]++;
  }

  std::map<uint32_t, uint32_t>::iterator it;
  fprintf(stderr, "HashString: mod 10\n");
  for (it = hash_info.begin(); it != hash_info.end(); ++it) {
    fprintf(stderr, "   %u:%u\n", it->first, it->second);
  }
  fprintf(stderr, "\n");
} /*}}}*/

struct Server {
  std::string id;
  std::string ip;
  int port;

  Server(const std::string& id, const std::string& ip, int port) : id(id), ip(ip), port(port) {}
};

TEST_D(HRWHash, Test_Normal_HRWHash, "测试HRW hash") { /*{{{*/
  using namespace base;

  std::vector<Server> servers = {Server("server1", "192.168.1.1", 12345), Server("server2", "192.168.1.2", 12345),
                                 Server("server3", "192.168.1.3", 12345)};

  std::vector<std::string> keys = {"test_key1", "test_key2", "test_key3", "test_key4"};
  for (auto key : keys) {
    Server server("", "", -1);
    Code ret = HRWHash(key, servers, &server);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "best server for key:%s is <%s, %s, %d>\n", key.c_str(), server.id.c_str(), server.ip.c_str(),
            server.port);
  }

  // Test the change of hash result after adding a server
  servers.push_back(Server("server4", "192.168.1.4", 12345));
  fprintf(stderr, "\nTest hash after add a new server:\n");
  for (auto key : keys) {
    Server server("", "", -1);
    Code ret = HRWHash(key, servers, &server);
    EXPECT_EQ(ret, kOk);
    fprintf(stderr, "best server for key:%s is <%s, %s, %d>\n", key.c_str(), server.id.c_str(), server.ip.c_str(),
            server.port);
  }
} /*}}}*/
