// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>

#include "lru_cache.h"

namespace store
{

LRUCache::LRUCache() : max_num_(0), time_interval_(0), cur_list_()
{/*{{{*/
    cur_list_.next = &cur_list_;
    cur_list_.pre = &cur_list_;
}/*}}}*/
    
LRUCache::~LRUCache()
{/*{{{*/
    caches_.clear();
    
    HandleNode *cur_node = cur_list_.next;
    while (cur_node != &cur_list_)
    {
        RemoveHandleNode(cur_node);

        delete cur_node;
        cur_node = cur_list_.next;
    }
}/*}}}*/


base::Code LRUCache::Init(uint32_t max_num, uint32_t time_interval)
{/*{{{*/
    max_num_ = max_num;
    time_interval_ = time_interval;

    return base::kOk;
}/*}}}*/

base::Code LRUCache::Put(const std::string &key, const std::string &value)
{/*{{{*/
    base::MutexLock ml(&mu_);

    std::map<std::string, HandleNode*>::iterator it = caches_.find(key);
    if (it != caches_.end())
    {
        it->second->value = value;
        it->second->access_time = time(NULL);

        RemoveHandleNode(it->second);
        InsertHandleNode(it->second);

        return base::kOk;
    }

    HandleNode *cur_node = new HandleNode();
    cur_node->key = key;
    cur_node->value = value;
    cur_node->access_time = time(NULL);
    InsertHandleNode(cur_node);

    caches_.insert(std::pair<std::string, HandleNode*>(key, cur_node));

    // Check the length and remove older datas if max_num_ is set
    if (max_num_ != 0 && caches_.size() > max_num_)
    {
        HandleNode *cur_node = cur_list_.pre;
        while (caches_.size() > max_num_)
        {
            RemoveHandleNode(cur_node);

            caches_.erase(cur_node->key);
            delete cur_node;

            cur_node = cur_list_.pre;
        }
    }

    return base::kOk;
}/*}}}*/

base::Code LRUCache::Get(const std::string &key, std::string *value)
{/*{{{*/
    if (value == NULL) return base::kInvalidParam;

    base::MutexLock ml(&mu_);

    // NOTE: here should not arrive
    if (max_num_ != 0 && caches_.size() > max_num_)
    {/*{{{*/
        HandleNode *cur_node = cur_list_.pre;
        while (caches_.size() > max_num_)
        {
            RemoveHandleNode(cur_node);

            caches_.erase(cur_node->key);
            delete cur_node;

            cur_node = cur_list_.pre;
        }
    }/*}}}*/

    std::map<std::string, HandleNode*>::iterator it = caches_.find(key);
    if (it != caches_.end())
    {/*{{{*/
        uint64_t cur_time = (uint64_t)time(NULL);
        if (time_interval_ != 0)
        {
            if ((cur_time - it->second->access_time) > time_interval_)
            {
                RemoveHandleNode(it->second);

                delete it->second;
                caches_.erase(it);

                return base::kNotFound;
            }
        }

        it->second->access_time = cur_time;
        RemoveHandleNode(it->second);
        InsertHandleNode(it->second);

        *value = it->second->value;

        return base::kOk;
    }/*}}}*/
    
    return base::kNotFound;
}/*}}}*/

base::Code LRUCache::Del(const std::string &key)
{/*{{{*/
    base::MutexLock ml(&mu_);

    std::map<std::string, HandleNode*>::iterator it = caches_.find(key);
    if (it != caches_.end())
    {
        RemoveHandleNode(it->second);
        delete it->second;

        caches_.erase(it);

        return base::kOk;
    }

    return base::kNotFound;
}/*}}}*/

base::Code LRUCache::RemoveHandleNode(HandleNode *cur_node)
{/*{{{*/
    if (cur_node == NULL) return base::kInvalidParam;

    cur_node->pre->next = cur_node->next;
    cur_node->next->pre = cur_node->pre;

    return base::kOk;
}/*}}}*/

base::Code LRUCache::InsertHandleNode(HandleNode *cur_node)
{/*{{{*/
    if (cur_node == NULL) return base::kInvalidParam;

    cur_node->next = cur_list_.next;
    cur_list_.next = cur_node;

    cur_node->pre = cur_node->next->pre;
    cur_node->next->pre = cur_node;

    return base::kOk;
}/*}}}*/

}

#ifdef _LRU_CACHE_MAIN_TEST_
int main(int argc, char *arg[])
{
    return 0;
}
#endif
