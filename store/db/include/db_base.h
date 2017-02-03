// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_DB_DB_BASE_H_
#define STORE_DB_DB_BASE_H_

#include <map>
#include <string>

#include <stdio.h>
#include <stdint.h>

#include "base/status.h"
#include "base/common.h"

namespace store
{

struct DBStatusInfo
{/*{{{*/
    uint32_t max_num;
    uint32_t used_cnt;
    uint64_t trx_id;
    std::map<uint32_t, uint32_t> same_hash_status;

    void Clear()
    {/*{{{*/
        max_num = 0;
        used_cnt = 0;
        trx_id = 0;
        same_hash_status.clear();
    }/*}}}*/

    void ToString(std::string *info)
    {/*{{{*/
        if (info == NULL) return;
        info->clear();

        char buf[base::kSmallBufLen] = {0};
        snprintf(buf, sizeof(buf)-1, "%u\n", (unsigned int)max_num);
        *info += std::string("max_num:") + buf;

        snprintf(buf, sizeof(buf)-1, "%u\n", (unsigned int)used_cnt);
        *info += std::string("used_cnt:") + buf;

        snprintf(buf, sizeof(buf)-1, "%llu\n", (unsigned long long)trx_id);
        *info += std::string("trx_id:") + buf;

        *info += std::string("num of same hash : num of num of same hash \n");
        std::map<uint32_t, uint32_t>::iterator it;
        for (it = same_hash_status.begin(); it != same_hash_status.end(); ++it)
        {
            snprintf(buf, sizeof(buf)-1, "%u", (unsigned int)it->first);
            *info += buf;

            snprintf(buf, sizeof(buf)-1, "%u\n", (unsigned int)it->second);
            *info += std::string("              : ") + buf;
        }
    }/*}}}*/
};/*}}}*/

/**
 * NOTE: This is database basic operation
 */
class DBBase
{
    public:
        DBBase() {}
        virtual ~DBBase() {}

    public:
        /**
         * NOTE: "Put" operation will set <key, value> into data base
         * 1. if version equals -1, then set <key, value> directly
         * 2. if version does not equal -1, then it will get <key, old_value, old_version> 
         *    and only set new value if version equals old_version, which is called CAS
         * 3. if this is the first time that key will be set, then version should be -1
         * 4. if you don't care CAS, then version may be not set
         */
        virtual base::Code Put(const std::string &key, const std::string &value, int64_t version=-1) = 0;

        /**
         * NOTE: "Get" operation will return value of key if it's exist
         * 1. if version if not NULL, it will be return at the same time     
         */
        virtual base::Code Get(const std::string &key, std::string *value, int64_t *version=NULL) = 0;

        /**
         * NOTE: "Del" operation will delete key if it's exist
         * 1. if version equals -1, then key will be deleted if it's exist
         * 2. if version doesn't equal -1, and if key exists, only if versions equals old_version, then
         *    the key will be deleted
         */
        virtual base::Code Del(const std::string &key, int64_t version=-1) = 0;

    public:
        virtual base::Code Init(const std::string &path) = 0;

        virtual base::Code GetStatus(DBStatusInfo *status_info) = 0;
};

}

#endif
