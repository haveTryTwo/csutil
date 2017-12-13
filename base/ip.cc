// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <netdb.h>
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

Code GetPeerIp(int sock_fd, std::string *peer_ip)
{/*{{{*/
    if (peer_ip == NULL) return kInvalidParam;

    struct sockaddr_in in_addr;
    socklen_t in_addr_len = sizeof(in_addr);

    int ret = getpeername(sock_fd, (struct sockaddr*)&in_addr, &in_addr_len);
    if (ret == -1) return kSocketError;

    char buf[kMaxLenOfIp];
    inet_ntop(AF_INET, (void*)&(in_addr.sin_addr), buf, sizeof(buf));
    peer_ip->assign(buf);

    return kOk;
}/*}}}*/

Code GetHostIpByName(const std::string &host_name, std::string *ip)
{/*{{{*/
    if (host_name.empty() || ip == NULL) return kInvalidParam;
    ip->clear();

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = NULL;
    int ret = getaddrinfo(host_name.c_str(), NULL, &hints, &res);
    if (ret != 0) return kGetHostByNameFailed;
    struct addrinfo *tmp_res = NULL;
    for (tmp_res = res; tmp_res != NULL; tmp_res = tmp_res->ai_next)
    {
        char buf[20] = {0};
        const char *pos = inet_ntop(AF_INET, &((sockaddr_in*)(tmp_res->ai_addr))->sin_addr,
            buf, sizeof(buf));
        if (pos == NULL) return kNetAddrConvertFailed;
        ip->append(buf);
        break;
    }
    freeaddrinfo(res);
    res = NULL;

    return kOk;
}/*}}}*/

Code GetHostIpByName(const std::string &host_name, uint32_t *ip)
{/*{{{*/
    if (host_name.empty() || ip == NULL) return kInvalidParam;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = NULL;
    int ret = getaddrinfo(host_name.c_str(), NULL, &hints, &res);
    if (ret != 0) return kGetHostByNameFailed;
    struct addrinfo *tmp_res = NULL;
    for (tmp_res = res; tmp_res != NULL; tmp_res = tmp_res->ai_next)
    {
        *ip = (((sockaddr_in*)(tmp_res->ai_addr))->sin_addr).s_addr;
        break;
    }
    freeaddrinfo(res);
    res = NULL;

    return kOk;
}/*}}}*/

Code GetHostIpByName(const std::string &host_name, std::deque<std::string> *ip)
{/*{{{*/
    if (host_name.empty() || ip == NULL) return kInvalidParam;
    ip->clear();

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = NULL;
    int ret = getaddrinfo(host_name.c_str(), NULL, &hints, &res);
    if (ret != 0) return kGetHostByNameFailed;
    struct addrinfo *tmp_res = NULL;
    for (tmp_res = res; tmp_res != NULL; tmp_res = tmp_res->ai_next)
    {
        char buf[20] = {0};
        const char *pos = inet_ntop(AF_INET, &((sockaddr_in*)(tmp_res->ai_addr))->sin_addr,
            buf, sizeof(buf));
        if (pos == NULL) return kNetAddrConvertFailed;
        ip->push_back(buf);
    }
    freeaddrinfo(res);
    res = NULL;

    return kOk;
}/*}}}*/

Code GetHostIpByName(const std::string &host_name, std::deque<uint32_t> *ip)
{/*{{{*/
    if (host_name.empty() || ip == NULL) return kInvalidParam;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = NULL;
    int ret = getaddrinfo(host_name.c_str(), NULL, &hints, &res);
    if (ret != 0) return kGetHostByNameFailed;
    struct addrinfo *tmp_res = NULL;
    for (tmp_res = res; tmp_res != NULL; tmp_res = tmp_res->ai_next)
    {
        ip->push_back((((sockaddr_in*)(tmp_res->ai_addr))->sin_addr).s_addr);
    }
    freeaddrinfo(res);
    res = NULL;

    return kOk;
}/*}}}*/

Code GetUIntIpByStr(const std::string &str_ip, uint32_t *uint_ip)
{/*{{{*/
    if (uint_ip == NULL) return kInvalidParam;
    struct in_addr in_addr_tmp;
    int ret = inet_pton(AF_INET, str_ip.c_str(), &in_addr_tmp);
    if (ret == 1)
    {
        *uint_ip = (uint32_t)in_addr_tmp.s_addr;
        return kOk;
    }

    return kInvalidIp;
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

    // check GetHostIpByName
    std::string host_name("translate.google.cn");
    std::string host_ip;
    ret = GetHostIpByName(host_name, &host_ip);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to gethostbyname! host_name:%s\n", host_name.c_str());
        return ret;
    }
    fprintf(stderr, "host_name:%s, host_ip:%s\n", host_name.c_str(), host_ip.c_str());

    uint32_t host_ip_int;
    ret = GetHostIpByName(host_name, &host_ip_int);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to gethostbyname! host_name:%s\n", host_name.c_str());
        return ret;
    }
    fprintf(stderr, "host_name:%s, host_ip_int:%#x, ", host_name.c_str(), host_ip_int);
    char *tmp = (char*)&host_ip_int;
    for (int i = 0; i < sizeof(host_ip_int); ++i)
    {
        fprintf(stderr, "%d.", *(uint8_t*)(tmp+i));
    }
    fprintf(stderr, "\n");

    std::deque<std::string> ips;
    ret = GetHostIpByName(host_name, &ips);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to gethostbyname! host_name:%s\n", host_name.c_str());
        return ret;
    }
    fprintf(stderr, "host_name:%s ", host_name.c_str());
    for (int i = 0; i < ips.size(); ++i)
    {
        fprintf(stderr, "host_ip:%s\n", ips[i].c_str());
    }

    std::deque<uint32_t> ips_int;
    ret = GetHostIpByName(host_name, &ips_int);
    if (ret != kOk)
    {
        fprintf(stderr, "Failed to gethostbyname! host_name:%s\n", host_name.c_str());
        return ret;
    }
    fprintf(stderr, "host_name:%s ", host_name.c_str());
    for (int i = 0; i < ips_int.size(); ++i)
    {
        fprintf(stderr, "host_ip:%#x\n", ips_int[i]);
    }

    std::string str_ip = "10.121.129.173";
    uint32_t uint_ip = 0;
    GetUIntIpByStr(str_ip, &uint_ip);
    fprintf(stderr, "str_ip:%s, uint_ip:%u\n\n", str_ip.c_str(), uint_ip);

    std::string str_ips[] = {"10.152.23.152", "10.58.131.74", "10.236.14.142", "10.236.21.27", "10.152.23.204"};
    for (int i = 0; i < 5; ++i)
    {
        GetUIntIpByStr(str_ips[i], &uint_ip);
        fprintf(stderr, "str_ip:%s, uint_ip:%u\n", str_ips[i].c_str(), uint_ip);
    }

    return 0;
}/*}}}*/
#endif
