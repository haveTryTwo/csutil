// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/hash.h"
#include "base/common.h"
#include "base/consistent_hash.h"

namespace base
{

ConsistentHash::ConsistentHash()
{
}

ConsistentHash::~ConsistentHash()
{
}

Code ConsistentHash::Put(const VirtualNode &node)
{/*{{{*/
    std::map<std::pair<uint32_t, uint16_t>, uint32_t>::iterator ips_it;
    ips_it = ips_.find(std::make_pair<uint32_t, uint16_t>(node.ip, node.port));
    if (ips_it != ips_.end())
    {
        if (ips_it->second == node.weight) return kOk;

        ips_it->second = node.weight;
    }
    else
    {
        ips_.insert(std::make_pair<std::pair<uint32_t, uint16_t>, uint32_t>(std::make_pair<uint32_t, uint16_t>(node.ip, node.port), node.weight));
    }

    Code ret = BalanceHashRing();
    return ret;
}/*}}}*/

Code ConsistentHash::Del(const VirtualNode &node)
{/*{{{*/
    std::map<std::pair<uint32_t, uint16_t>, uint32_t>::iterator ips_it;
    ips_it = ips_.find(std::make_pair<uint32_t, uint16_t>(node.ip, node.port));
    if (ips_it != ips_.end())
    {
        ips_.erase(ips_it);
    }

    Code ret = BalanceHashRing();
    return ret;
}/*}}}*/

Code ConsistentHash::Get(const std::string &key, VirtualNode *virtual_node)
{/*{{{*/
    if (virtual_node == NULL) return kInvalidParam;
    if (hash_ring_.empty()) return kRingEmpty;

    uint32_t hash_key = Murmur32(key, kConsistentHashSeed);
    std::map<uint32_t, VirtualNode>::const_iterator const_ring_it;
//    const_ring_it = GetEqualOrUpperBound(hash_ring_, hash_key);
    const_ring_it = hash_ring_.lower_bound(hash_key);

    // ring back
    if (const_ring_it == hash_ring_.end())
    {
        const_ring_it = hash_ring_.begin();
    }

    *virtual_node = const_ring_it->second;

    return kOk;
}/*}}}*/

Code ConsistentHash::GetPrintInfo(std::string *info)
{/*{{{*/
    if (info == NULL) return kInvalidParam;

    std::map<std::pair<uint32_t, uint16_t>, uint32_t>::iterator ips_it;
    info->append(std::string("--------Ip info:----------------------\n"));
    for (ips_it = ips_.begin(); ips_it != ips_.end(); ++ips_it)
    {
        char buf[kSmallBufLen] = {0};
        int r = snprintf(buf, sizeof(buf)-1, "%u:%u:%u\n", (unsigned int)ips_it->first.first,
            (unsigned int)ips_it->first.second, (unsigned int)ips_it->second);
        info->append(buf, r);
    }

    info->append(std::string("\n--------Hash Ringh info:-------------\n"));
    std::map<uint32_t, VirtualNode>::iterator ring_it;
    for (ring_it = hash_ring_.begin(); ring_it != hash_ring_.end(); ++ring_it)
    {
        char buf[kSmallBufLen] = {0};
        int r = snprintf(buf, sizeof(buf)-1, "%u =><%u:%u:%u>\n", (unsigned int)ring_it->first,
            (unsigned int)ring_it->second.ip, (unsigned int)ring_it->second.port,
            (unsigned int)ring_it->second.weight);
        info->append(buf, r);
    }

    return kOk;
}/*}}}*/


Code ConsistentHash::BalanceHashRing()
{/*{{{*/
    std::map<uint32_t, VirtualNode>::iterator ring_it;
    std::map<std::pair<uint32_t, uint16_t>, uint32_t>::iterator ips_it;

    hash_ring_.clear();
    for (ips_it = ips_.begin(); ips_it != ips_.end(); ++ips_it)
    {
        for (uint32_t virtual_num = 0; virtual_num < ips_it->second; ++virtual_num)
        {
            char buf[kSmallBufLen] = {0};
            int r = snprintf(buf, sizeof(buf)-1, "%u:%u:%u", (unsigned int)ips_it->first.first,
                (unsigned int)ips_it->first.second, (unsigned int)virtual_num);
            uint32_t hash_key = Murmur32(std::string(buf, r), kConsistentHashSeed);

            VirtualNode new_node(ips_it->first.first, ips_it->first.second, ips_it->second);
            ring_it = hash_ring_.find(hash_key);
            if (ring_it != hash_ring_.end())
            {
                bool is_replace = CheckIfReplace(new_node, ring_it->second);
                if (is_replace)
                {
                    hash_ring_[hash_key] = new_node;
                }
            }
            else
            {
                hash_ring_.insert(std::make_pair<uint32_t, VirtualNode>(hash_key, new_node));
            }
        }
    }

    return kOk;
}/*}}}*/

bool ConsistentHash::CheckIfReplace(const VirtualNode &new_node, const VirtualNode &old_node)
{/*{{{*/
    if (new_node.weight > old_node.weight)
    {
        return true;
    }
    else if (new_node.weight < old_node.weight)
    {
        return false;
    }

    if (new_node.ip >  old_node.ip)
    {
        return true;
    }
    else if (new_node.ip < old_node.ip)
    {
        return false;
    }

    if (new_node.port > old_node.port)
    {
        return true;
    }
    else
    {
        return false;
    }

    return false;
}/*}}}*/

}
