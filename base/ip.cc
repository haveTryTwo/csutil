// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <unistd.h>
#include <net/if.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "base/ip.h"
#include "base/common.h"

namespace base
{

Code GetLocalIp(const std::string &interface, std::string *ip)
{/*{{{*/
    if (ip == NULL) return kInvalidParam;

    struct in_addr sin_addr;
    Code ret = GetLocalIp(interface, &sin_addr);
    if (ret != kOk) return ret;

    char buf[kMaxLenOfIp];
    inet_ntop(AF_INET, (void*)&sin_addr, buf, sizeof(buf));
    ip->assign(buf);
   
    return kOk;
}/*}}}*/

Code GetLocalIp(const std::string &interface, struct in_addr *sin_addr)
{/*{{{*/
    if (sin_addr == NULL) return kInvalidParam;

    struct ifreq req;
    strncpy(req.ifr_name, interface.c_str(), IFNAMSIZ);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) return kSocketError;

    int ret = ioctl(sock_fd, SIOCGIFADDR, (char*)&req);
    if (ret < 0)
    {
        close(sock_fd);
        return kIOCtlError;
    }

    *sin_addr = ((struct sockaddr_in*)(&req.ifr_addr))->sin_addr;
    close(sock_fd);
    
    return kOk;
}/*}}}*/

Code GetInterfaces(std::vector<std::string> *ifs)
{/*{{{*/
    if (ifs == NULL) return kInvalidParam;

    struct ifaddrs *if_addrs = NULL;

    int ret = getifaddrs(&if_addrs);
    if (ret == -1)
        return kGetIfsError;

    struct ifaddrs *tmp_addrs = if_addrs;
    while (tmp_addrs != NULL)
    {
        if (tmp_addrs->ifa_addr->sa_family == AF_INET)
        {
            ifs->push_back(tmp_addrs->ifa_name);
        }
        tmp_addrs = tmp_addrs->ifa_next;
    }
    freeifaddrs(if_addrs);

    return kOk;
}/*}}}*/

Code GetInterfaces(std::map<std::string, struct in_addr> *if_in_addrs)
{/*{{{*/
    if (if_in_addrs == NULL) return kInvalidParam;

    struct ifaddrs *if_addrs = NULL;

    int ret = getifaddrs(&if_addrs);
    if (ret == -1)
        return kGetIfsError;

    struct ifaddrs *tmp_addrs = if_addrs;
    while (tmp_addrs != NULL)
    {
        if (tmp_addrs->ifa_addr->sa_family == AF_INET)
        {
            if_in_addrs->insert(std::make_pair<std::string, struct in_addr>
                        (std::string(tmp_addrs->ifa_name),
                        ((struct sockaddr_in*)(tmp_addrs->ifa_addr))->sin_addr));

        }
        tmp_addrs = tmp_addrs->ifa_next;
    }
    freeifaddrs(if_addrs);

    return kOk;
}/*}}}*/

Code GetInterfaces(std::map<std::string, std::string> *if_ips)
{/*{{{*/
    if (if_ips == NULL) return kInvalidParam;

    struct ifaddrs *if_addrs = NULL;

    int ret = getifaddrs(&if_addrs);
    if (ret == -1)
        return kGetIfsError;

    char buf[16];
    struct ifaddrs *tmp_addrs = if_addrs;
    while (tmp_addrs != NULL)
    {
        if (tmp_addrs->ifa_addr->sa_family == AF_INET)
        {
            inet_ntop(AF_INET, &((struct sockaddr_in*)(tmp_addrs->ifa_addr))->sin_addr, 
                    buf, sizeof(buf));
            if_ips->insert(std::make_pair<std::string, std::string>(tmp_addrs->ifa_name, buf));

        }
        tmp_addrs = tmp_addrs->ifa_next;
    }
    freeifaddrs(if_addrs);

    return kOk;
}/*}}}*/

}

#ifdef _IP_MAIN_TEST_
#include <stdio.h>
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

    std::string interface("en0");
    std::string ip;

    Code ret = GetLocalIp(interface, &ip);
    fprintf(stderr, "ret:%d, ip of %s is %s\n", ret, interface.c_str(), ip.c_str());


    std::vector<std::string> ifs;
    typedef std::vector<std::string>::iterator VecIter;
    ret = GetInterfaces(&ifs);
    for (VecIter it = ifs.begin(); it != ifs.end(); ++it)
    {
        fprintf(stderr, "%s\n", (*it).c_str());
    }

    std::map<std::string, struct in_addr> if_addrs;
    typedef std::map<std::string, struct in_addr>::iterator MapIter; 
    ret = GetInterfaces(&if_addrs);
    for (MapIter it = if_addrs.begin(); it != if_addrs.end(); ++it)
    {
        char buf[16];
        inet_ntop(AF_INET, &(it->second), buf, sizeof(buf));
        fprintf(stderr, "%s : %s\n", it->first.c_str(), buf);
    }

    std::map<std::string, std::string> if_ips;
    typedef std::map<std::string, std::string>::iterator MapStrIter;
    ret = GetInterfaces(&if_ips);
    for (MapStrIter it = if_ips.begin(); it != if_ips.end(); ++it)
    {
        fprintf(stderr, "%s : %s\n", it->first.c_str(), it->second.c_str());
    }
    return 0;
}/*}}}*/
#endif
