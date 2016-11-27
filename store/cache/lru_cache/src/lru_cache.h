// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_CACHE_LRU_CHACHE_H_ 
#define STORE_CACHE_LRU_CHACHE_H_

#include <map>
#include <string>

#include <stdint.h>

#include "base/mutex.h"
#include "base/status.h"

namespace store
{

/**
 * Note: here support lru by number or lru by time or by number and time at the same time:
 *  1. no lru : default mode
 *  2. lru by number
 *  3. lru by time
 *  4. lru by number and b time
 *
 *  1) LRU by number
 *      It will be run when put one data, the length of cache will be checked, if it's longer than
 *      max_num_, then the older datas will be deleted;
 *
 *  2) LRU by time
 *      It will be run when get one data, the time of data will be checked, if it's older than current
 *      time - timer_interval, then current data will be deleted;
 */

struct HandleNode
{
    HandleNode *pre;
    HandleNode *next;

    std::string key;
    std::string value;
    uint64_t access_time;

    HandleNode() : pre(NULL), next(NULL), key(""), value(""), access_time(0)
    {}
    
    ~HandleNode() {}
};

class LRUCache
{
    public:
        LRUCache();
        ~LRUCache();

        base::Code Init(uint32_t max_num, uint32_t time_interval);

    public:
        base::Code Put(const std::string &key, const std::string &value);
        base::Code Get(const std::string &key, std::string *value);
        base::Code Del(const std::string &key);

    private:
        base::Code RemoveHandleNode(HandleNode *cur_node);
        base::Code InsertHandleNode(HandleNode *cur_node);

    private:
        uint32_t max_num_;          // lru by number
        uint32_t time_interval_;    // lru by time

        std::map<std::string, HandleNode*> caches_;
        HandleNode cur_list_;

        base::Mutex mu_; // lock when Put/Get/Del
};


}

#endif
