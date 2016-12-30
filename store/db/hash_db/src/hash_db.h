// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_DB_HASH_DB_H_
#define STORE_DB_HASH_DB_H_

#include <string>

#include <stdint.h>
#include <string.h>

#include "base/status.h"
#include "store/db/include/db_base.h"

namespace store
{

const uint32_t kPreKeySize = 8;

// Num of keys should be less than kMaxKeysNum, or the possibility of conflicting is larger
const uint32_t kMaxKeysNum = 1024 * 1024;   
const uint32_t kMaxDataSize = 8 * 1024 * 1024;

class HashDB : public DBBase
{
    public:
        HashDB();
        virtual ~HashDB();

    public:
        /**
         * NOTE: "Put" operation will set <key, value> into data base
         * 1. if version equals -1, then set <key, value> directly
         * 2. if version does not equal -1, then it will get <key, old_value, old_version> 
         *    and only set new value if version equals old_version, which is called CAS
         * 3. if this is the first time that key will be set, then version should be 0
         * 4. if you don't care CAS, then version may be not set
         * 5. the first version of data is 1
         */
        virtual base::Code Put(const std::string &key, const std::string &value, int64_t version=-1);

        /**
         * NOTE: "Get" operation will return value of key if it's exist
         * 1. if version if not NULL, it will be return at the same time     
         */
        virtual base::Code Get(const std::string &key, std::string *value, int64_t *version=NULL);

        /**
         * NOTE: "Del" operation will delete key if it's exist
         * 1. if version equals -1, then key will be deleted if it's exist
         * 2. if version doesn't equal -1, and if key exists, only if versions equals old_version, then
         *    the key will be deleted
         */
        virtual base::Code Del(const std::string &key, int64_t version=-1);

    public:
        virtual base::Code Init(const std::string &path);

    public:
#pragma pack(push)
#pragma pack(1)
        struct Info
        {/*{{{*/
            uint32_t max_num;
            uint32_t used_cnt;
            uint64_t trx_id;
        };/*}}}*/

        struct Bucket
        {/*{{{*/
            uint64_t version;
            uint64_t next_pos;
            uint32_t key_hash; //crc key
            char pre_key[kPreKeySize];
            uint32_t key_size;
            uint32_t value_size;
            uint64_t data_pos;

            void Clear()
            {/*{{{*/
                version = 0;
                next_pos = 0;
                key_hash = 0;
                memset(pre_key, 0, sizeof(pre_key));
                key_size = 0;
                value_size = 0;
                data_pos = 0;
            }/*}}}*/
        };/*}}}*/

        /**
         * NOTE: index file format:
         *      | Info  |start_pos|...|start_pos|bucket|...|bucket|
         *      \ info / \      hash_dir       / \     buckets    /
         */
        struct Index
        {/*{{{*/
            Info info;
            uint64_t hash_dir[kMaxKeysNum];
            Bucket buckets[kMaxKeysNum];
        };/*}}}*/

        /**
         * NOTE: <key, value> format in data file:
         *      |total_size||key_size|key||value_size|value|version|crc|
         *      total_size = uint32_t + key_size + uint32_t + value_size
         *      sizeof(version) = sizeof(uint64_t)
         *      sizeof(crc) = sizeof(uint32_t)
         */
        struct DataValue
        {/*{{{*/
            uint32_t total_size;
            uint32_t key_size;
            std::string key;
            uint32_t value_size;
            std::string value;
            uint64_t version;
            uint32_t crc;

            void Clear()
            {/*{{{*/
                total_size = 0;
                key_size = 0;
                key.clear();
                value_size = 0;
                value.clear();
                version = 0;
                crc = 0;
            }/*}}}*/
        };/*}}}*/
#pragma pack(pop)

    public:
        virtual base::Code GetStatus(DBStatusInfo *status_info);

    private:
        base::Code Destroy();
        base::Code InitIndex(bool is_first);
        base::Code EncodeInfo(const Info &info, std::string *str);
        base::Code DecodeInfo(const std::string &str, Info *info);
        base::Code EncodeBucket(const Bucket &bucket, std::string *str);
        base::Code DecodeBucket(const std::string &str, Bucket *bucket);
        base::Code EncodeDataValue(const DataValue &data_value, std::string *str);
        base::Code DecodeDataValue(const std::string &str, DataValue *data_value);

    private:
        // NOTE: index file path: path_ + ".idx"
        //       data file path:  path_ + ".dat"
        std::string path_;
        FILE *index_fp_;
        FILE *data_fp_;
        Index *index_;
        char *buf_;
        std::string stat_path_;
};

}

#endif
