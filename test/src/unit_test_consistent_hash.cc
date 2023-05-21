
// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

#include "base/consistent_hash.h"
#include "base/hash.h"
#include "base/ip.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(ConsistentHash, Test_Normal_Put) { /*{{{*/
  using namespace base;

  std::string ips[] = {"10.101.102.131", "10.201.201.201", "10.151.151.151"};
  ConsistentHash consistent_hash;
  for (uint32_t ip_index = 0; ip_index < sizeof(ips) / sizeof(ips[0]); ++ip_index) {
    uint32_t uint_ip = 0;
    Code ret = GetUIntIpByStr(ips[ip_index], &uint_ip);
    EXPECT_EQ(kOk, ret);
    VirtualNode node(uint_ip, 8080, 10);

    ret = consistent_hash.Put(node);
    EXPECT_EQ(kOk, ret);

    std::string ring_info;
    ret = consistent_hash.GetPrintInfo(&ring_info);
    EXPECT_EQ(kOk, ret);
    //        struct in_addr uint_ip_addr = {uint_ip};
    //        fprintf(stderr, "Add ip:%s, uint_ip:%u\n", inet_ntoa(uint_ip_addr),
    //        (unsigned)uint_ip); fprintf(stderr, "%s\n", ring_info.c_str());
  }
} /*}}}*/

TEST(ConsistentHash, Test_Normal_Get) { /*{{{*/
  using namespace base;

  std::string ips[] = {"10.101.102.131", "10.201.201.201", "10.151.151.151"};
  ConsistentHash consistent_hash;
  Code ret = kOk;
  for (uint32_t ip_index = 0; ip_index < sizeof(ips) / sizeof(ips[0]); ++ip_index) {
    uint32_t uint_ip = 0;
    ret = GetUIntIpByStr(ips[ip_index], &uint_ip);
    EXPECT_EQ(kOk, ret);
    VirtualNode node(uint_ip, 8080, 10);

    ret = consistent_hash.Put(node);
    EXPECT_EQ(kOk, ret);

    std::string ring_info;
    ret = consistent_hash.GetPrintInfo(&ring_info);
    EXPECT_EQ(kOk, ret);
    //        struct in_addr uint_ip_addr = {uint_ip};
    //        fprintf(stderr, "Add ip:%s, uint_ip:%u\n", inet_ntoa(uint_ip_addr),
    //        (unsigned)uint_ip); fprintf(stderr, "%s\n", ring_info.c_str());
  }

  std::string keys[] = {"key1", "key2", "key3", "key4", "key5", "key6"};
  uint32_t uint_ips[] = {2543294218, 2543294218, 3385444618, 3385444618, 2543294218, 2543294218};
  for (uint32_t key_index = 0; key_index < sizeof(keys) / sizeof(keys[0]); ++key_index) {
    VirtualNode key_node;
    ret = consistent_hash.Get(keys[key_index], &key_node);
    EXPECT_EQ(kOk, ret);
    EXPECT_EQ(uint_ips[key_index], key_node.ip);
    uint32_t num = Murmur32(keys[key_index], 0x0523);
    //        fprintf(stderr, "key:%s, hash num:%u\n", keys[key_index].c_str(), num);
    //        fprintf(stderr, "key:%s, <ip:port:weight>:<%u:%u:%u>\n", keys[key_index].c_str(),
    //            (unsigned)key_node.ip, (unsigned)key_node.port, (unsigned)key_node.weight);
  }
} /*}}}*/

TEST(ConsistentHash, Test_Normal_ChangeWeight) { /*{{{*/
  using namespace base;

  std::string ips[] = {"10.101.102.131", "10.201.201.201", "10.151.151.151"};
  ConsistentHash consistent_hash;
  for (uint32_t ip_index = 0; ip_index < sizeof(ips) / sizeof(ips[0]); ++ip_index) {
    uint32_t uint_ip = 0;
    Code ret = GetUIntIpByStr(ips[ip_index], &uint_ip);
    EXPECT_EQ(kOk, ret);
    VirtualNode node(uint_ip, 8080, 10);

    ret = consistent_hash.Put(node);
    EXPECT_EQ(kOk, ret);
    std::string ring_info;
    ret = consistent_hash.GetPrintInfo(&ring_info);
    EXPECT_EQ(kOk, ret);
    //        struct in_addr uint_ip_addr = {uint_ip};
    //        fprintf(stderr, "Add ip:%s, uint_ip:%u\n", inet_ntoa(uint_ip_addr),
    //        (unsigned)uint_ip); fprintf(stderr, "%s\n", ring_info.c_str());

    // Change weight
    node.weight = ip_index + 5;
    ret = consistent_hash.Put(node);
    EXPECT_EQ(kOk, ret);
    ring_info.clear();
    ret = consistent_hash.GetPrintInfo(&ring_info);
    EXPECT_EQ(kOk, ret);
    //        uint_ip_addr.s_addr = uint_ip;
    //        fprintf(stderr, "Mod ip:%s, uint_ip:%u\n", inet_ntoa(uint_ip_addr),
    //        (unsigned)uint_ip); fprintf(stderr, "%s\n", ring_info.c_str());
  }
} /*}}}*/
