// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CONSISTENT_HASH_H_
#define BASE_CONSISTENT_HASH_H_

#include <map>
#include <string>
#include <utility>

#include <stdint.h>

#include "base/status.h"

namespace base {

struct VirtualNode { /*{{{*/
  VirtualNode() : ip(0), port(0), weight(0) {}

  VirtualNode(uint32_t ip1, uint16_t port1, uint32_t weight1)
      : ip(ip1), port(port1), weight(weight1) {}

  uint32_t ip;
  uint16_t port;
  uint32_t weight;
}; /*}}}*/

class ConsistentHash {
 public:
  ConsistentHash();
  ~ConsistentHash();

 public:
  Code Put(const VirtualNode &node);
  Code Del(const VirtualNode &node);

  Code Get(const std::string &key, VirtualNode *virtual_node);

  Code GetPrintInfo(std::string *info);

 private:
  Code BalanceHashRing();
  bool CheckIfReplace(const VirtualNode &new_node, const VirtualNode &old_node);

 private:
  std::map<uint32_t, VirtualNode> hash_ring_;
  std::map<std::pair<uint32_t, uint16_t>, uint32_t> ips_;
};

}  // namespace base

#endif
