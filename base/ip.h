// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_IP_H_
#define BASE_IP_H_

#include <map>
#include <string>
#include <vector>

#include <stdint.h>

#include "base/status.h"

namespace base
{

/**
 * Get the string ip (like "127.0.0.1") according the interface;
 * The interface may be "eth0", "eth1", "lo" and so
 */
Code GetLocalIp(const std::string &interface, std::string *ip);

Code GetLocalIp(const std::string &interface, struct in_addr *sin_addr);

Code GetInterfaces(std::vector<std::string> *ifs);

Code GetInterfaces(std::map<std::string, struct in_addr> *if_in_addrs);

Code GetInterfaces(std::map<std::string, std::string> *if_ips);

Code GetPeerIp(int sock_fd, std::string *peer_ip);

Code GetHostIpByName(const std::string &host_name, std::string *ip);

Code GetHostIpByName(const std::string &host_name, uint32_t *ip);

}

#endif
