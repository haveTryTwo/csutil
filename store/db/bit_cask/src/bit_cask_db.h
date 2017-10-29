// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_DB_BIT_CASK_DB_H_
#define STORE_DB_BIT_CASK_DB_H_

#include <map>
#include <string>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "base/status.h"
#include "base/common.h"
#include "store/db/include/db_base.h"

namespace store
{

const uint32_t kSingleFileSize = 10 * 1024 * 1024;  // Max of single file size is 10MB
const uint32_t kMaxDataSize = 10 * 1024 * 1024;     // Max size of a data
const uint32_t kMaxKeySize = 1024;                  // Max size of a key
const uint32_t kMaxKeysNum = 1024 * 1024; 

// Note: data file name will be:            data.00000000000000001
//       merge data file name wil be: merge_data.00000000000000001
const std::string kSourceDataFilePrefix = "data";
const std::string kMergeDataFilePrefix = "merge_data";
const int kDataFileSuffixLen = 17;
const uint64_t kMaxFilesNum = (uint64_t)pow(base::kTen, kDataFileSuffixLen) - 1;
const uint32_t kFileNamePartsSize = 2;
const uint32_t kInitSuffixNum = 1;

enum DataInfoFlag
{
    kBCInvalidFlag    = 0,
    kBCExistFlag      = 1,
    kBCDelFlag        = 2,
};

class BitCaskDB : public DBBase
{
    public:
        BitCaskDB();
        virtual ~BitCaskDB();

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
        virtual base::Code Init(const std::string &dir_path);

    public:
#pragma pack(push)
#pragma pack(1)
        struct Info
        {/*{{{*/
            uint64_t max_num;
            uint64_t used_cnt;
            uint64_t del_cnt;
            uint64_t trx_id;
        };/*}}}*/

        struct Bucket
        {/*{{{*/
            FILE *fp;
            uint32_t del_flag;
            uint32_t key_size;
            uint32_t value_size;
            uint64_t data_pos;
            uint32_t time_sec;  // create time of second
            uint32_t time_nsec; // create time of nanosecond
            uint64_t version;

            Bucket()
            {/*{{{*/
                fp = NULL;
                del_flag = kBCInvalidFlag;
                key_size = 0;
                value_size = 0;
                data_pos = 0;
                time_sec = 0;
                time_nsec = 0;
                version = 0;
            }/*}}}*/

            Bucket(const Bucket &bucket)
            {/*{{{*/
                fp = bucket.fp;
                del_flag = bucket.del_flag;
                key_size = bucket.key_size;
                value_size = bucket.value_size;
                data_pos = bucket.data_pos;
                time_sec = bucket.time_sec;
                time_nsec = bucket.time_nsec;
                version = bucket.version;
            }/*}}}*/

            Bucket& operator= (const Bucket &bucket)
            {/*{{{*/
                fp = bucket.fp;
                del_flag = bucket.del_flag;
                key_size = bucket.key_size;
                value_size = bucket.value_size;
                data_pos = bucket.data_pos;
                time_sec = bucket.time_sec;
                time_nsec = bucket.time_nsec;
                version = bucket.version;
                return *this;
            }/*}}}*/

            ~Bucket()
            {/*{{{*/
                PartClear();
            }/*}}}*/

            void PartClear()
            {/*{{{*/
                if (fp != NULL) 
                {
                    fp = NULL; // Not fclose(fp), it should be done by files_;
                }
                del_flag = kBCInvalidFlag;
                key_size = 0;
                value_size = 0;
                data_pos = 0;
                time_sec = 0;
                time_nsec = 0;
                version = 0;
            }/*}}}*/
        };/*}}}*/

        /**
         * NOTE: <key, value> format in data file:
         *      |crc||del_flag|time_sec|time_nsec|version|key_size||value_size|key|value|
         *      sizeof(version) = sizeof(uint64_t)
         *      sizeof(crc) = sizeof(uint32_t)
         */
        struct DataValue
        {/*{{{*/
            uint32_t crc;
            uint32_t del_flag;
            uint32_t time_sec;
            uint32_t time_nsec;
            uint64_t version;
            uint32_t key_size;
            uint32_t value_size;
            std::string key;
            std::string value;

            void Clear()
            {/*{{{*/
                crc = 0;
                del_flag = kBCInvalidFlag;
                time_sec = 0;
                time_nsec = 0;
                version = 0;
                key_size = 0;
                value_size = 0;
                key.clear();
                value.clear();
            }/*}}}*/
        };/*}}}*/
#pragma pack(pop)

    public:
        virtual base::Code GetStatus(DBStatusInfo *status_info);

    private:
        base::Code Destroy();
        base::Code OpenAndReadFiles(const std::vector<std::string> &files_name, const std::string &prefix, uint64_t *cur_suffix_num);
        base::Code OpenAndReadFile(const std::string &file_name, const std::string &mode);
        base::Code OpenNextFile(const std::string &prefix, uint64_t cur_suffix_num);
        base::Code CheckIsFileFull(const std::string &prefix, uint64_t cur_suffix_num, bool *full);
        base::Code ReadData(DataValue *data_value, FILE *fp);
        base::Code WriteData(const DataValue &data_value, uint64_t *cur_pos, FILE *fp);
        base::Code CheckValueIsNew(const Bucket &first_bucket, const Bucket &second_bucket, bool *is_new);
        base::Code CheckValueIsNew(uint32_t first_sec, uint32_t first_nsec, uint32_t second_sec, uint32_t second_nsec, bool *is_new);
        base::Code GetFileName(const std::string &prefix, uint64_t cur_suffix_num, std::string *file_name);

        base::Code SetValue(const std::string &key, const std::string &value, int flag, int64_t version);

        base::Code EncodeDataValueHead(const DataValue &data_value, std::string *str);
        base::Code DecodeDataValueHead(const std::string &str, DataValue *data_value);
        base::Code EncodeDataValue(const DataValue &data_value, std::string *str);
        base::Code DecodeDataValue(const std::string &str, DataValue *data_value);

    private:
        // NOTE: files in directory: source data files and merge data files
        std::string dir_path_;
        Info info_;
        std::map<std::string, Bucket> index_;
        uint64_t cur_data_file_suffix_num_;
        uint64_t cur_merge_data_file_suffix_num_;
        char *buf_;
        std::map<std::string, FILE*> files_;     // Data files and fp after fopen
        std::string stat_path_;     // TODO: To write stat info to file
};

}

#endif
