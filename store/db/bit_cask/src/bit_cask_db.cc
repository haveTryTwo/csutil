// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "base/log.h"
#include "base/int.h"
#include "base/util.h"
#include "base/hash.h"
#include "base/time.h"
#include "base/coding.h"
#include "base/file_util.h"

#include "bit_cask_db.h"

namespace store
{

class FilesSort
{/*{{{*/
    public:
        bool operator()(const std::string &path1, const std::string &path2)
        {
            if (path1.compare(path2) > 0)
                return true;
            else
                return false;
        }
};/*}}}*/

BitCaskDB::BitCaskDB() : dir_path_(""), cur_data_file_suffix_num_(0), cur_merge_data_file_suffix_num_(0)
{/*{{{*/
    memset((void*)(&info_), 0, sizeof(info_));
    buf_ = new char[kMaxDataSize+kMaxKeySize];
    memset(buf_, 0, sizeof(kMaxDataSize));
}/*}}}*/

BitCaskDB::~BitCaskDB()
{/*{{{*/
    Destroy();

    if (buf_ != NULL)
    {
        delete []buf_;
        buf_ = NULL;
    }
}/*}}}*/

base::Code BitCaskDB::Put(const std::string &key, const std::string &value, int64_t version/*=-1*/)
{/*{{{*/
    return SetValue(key, value, kBCExistFlag, version);
}/*}}}*/

base::Code BitCaskDB::Get(const std::string &key, std::string *value, int64_t *version/*=NULL*/)
{/*{{{*/
    if (value == NULL) return base::kInvalidParam;

    base::Code ret = base::kOk;
    std::map<std::string, Bucket>::iterator index_it = index_.find(key);
    if (index_it == index_.end()) return base::kNotFound;

    Bucket *cur_bucket = &(index_it->second);
    if (cur_bucket->del_flag == kBCDelFlag) return base::kNotFound;

    DataValue cur_data_value;
    fseek(cur_bucket->fp, cur_bucket->data_pos, SEEK_SET);
    ret = ReadData(&cur_data_value, cur_bucket->fp);
    if (ret != base::kOk) return ret;
    *value = cur_data_value.value;
    if (version != NULL)
        *version = cur_data_value.version;

    return ret;
}/*}}}*/

base::Code BitCaskDB::Del(const std::string &key, int64_t version/*=-1*/)
{/*{{{*/
    return SetValue(key, "", kBCDelFlag, version);
}/*}}}*/

base::Code BitCaskDB::Init(const std::string &dir_path)
{/*{{{*/
    if (dir_path.empty()) return base::kInvalidParam;

    dir_path_ = dir_path;
    info_.max_num = kMaxKeysNum;

    std::vector<std::string> files_name;
    base::Code ret = base::GetNormalFilesNameWithOutSort(dir_path_, &files_name);
    if (ret != base::kOk) return ret;

    std::sort(files_name.begin(), files_name.end(), FilesSort());

    ret = OpenAndReadFiles(files_name, kSourceDataFilePrefix, &cur_data_file_suffix_num_);
    if (ret != base::kOk) return ret;
    
    ret = OpenAndReadFiles(files_name, kMergeDataFilePrefix, &cur_merge_data_file_suffix_num_);
    if (ret != base::kOk) return ret;

    return ret;
}/*}}}*/

base::Code BitCaskDB::GetStatus(DBStatusInfo *status_info)
{/*{{{*/
    if (status_info == NULL) return base::kInvalidParam;

    status_info->max_num = info_.max_num;
    status_info->used_cnt = info_.used_cnt;
    status_info->trx_id = info_.trx_id;

    return base::kOk;
}/*}}}*/

base::Code BitCaskDB::Destroy()
{/*{{{*/
    std::map<std::string, FILE*>::iterator file_it = files_.begin();
    for (; file_it != files_.end(); ++file_it)
    {
        FILE *cur_fp = file_it->second;
        fclose(cur_fp);
        file_it->second = NULL;
    }

    return base::kOk;
}/*}}}*/

base::Code BitCaskDB::OpenAndReadFiles(const std::vector<std::string> &files_name, const std::string &prefix, uint64_t *cur_suffix_num)
{/*{{{*/
    if (cur_suffix_num == NULL) return base::kInvalidParam;

    bool first_file_found = false;
    std::vector<std::string>::const_iterator files_it;
    base::Code ret = base::kOk;
    for (files_it = files_name.begin(); files_it != files_name.end(); ++files_it)
    {/*{{{*/
        std::string cur_file_name = *files_it;
        std::deque<std::string> file_name_parts;
        ret = base::Strtok(cur_file_name, base::kDot, &file_name_parts);
        if (ret != base::kOk) return ret;
        if (file_name_parts.size() != kFileNamePartsSize)
            return base::kInvalidBitCaskFileName;

        std::string first_name_part = file_name_parts[0];
        std::string second_name_part = file_name_parts[1];
        if (first_name_part == prefix)
        {/*{{{*/
            std::string open_mode = "r";
            uint64_t file_suffix_num = 0;
            ret = base::GetUInt64(second_name_part, &file_suffix_num);
            if (ret != base::kOk) return ret;
            if (!first_file_found)
            {
                first_file_found = true;
                *cur_suffix_num = file_suffix_num;
                open_mode = "a+";
            }
            else
            {
                if (*cur_suffix_num <= file_suffix_num)
                    return base::kInvalidBitCaskFileName;
            }

            ret = OpenAndReadFile(cur_file_name, open_mode);
            if (ret != base::kOk) return ret;
        }/*}}}*/
    }/*}}}*/

    if (!first_file_found)
    {
        *cur_suffix_num = kInitSuffixNum;
        ret = OpenNextFile(prefix, *cur_suffix_num-1);
        if (ret != base::kOk) return ret;
    }

    return ret;
}/*}}}*/

base::Code BitCaskDB::OpenAndReadFile(const std::string &file_name, const std::string &mode)
{/*{{{*/
    std::string file_path = dir_path_+base::kSlashStr+file_name;
    FILE *cur_fp = fopen(file_path.c_str(), mode.c_str());
    if (cur_fp == NULL) return base::kOpenFileFailed;

    files_.insert(std::pair<std::string, FILE*>(file_name, cur_fp));

    uint64_t cur_pos = 0;
    fseek(cur_fp, cur_pos, SEEK_SET);
    base::Code ret = base::kOk;
    while (true)
    {
        DataValue cur_data_value;
        ret = ReadData(&cur_data_value, cur_fp);
        if (ret == base::kFileIsEnd)
        {
            return base::kOk;
        }
        else if (ret != base::kOk)
        {
            return ret;
        }

        Bucket cur_bucket;
        cur_bucket.fp = cur_fp; 
        cur_bucket.del_flag = cur_data_value.del_flag;
        cur_bucket.key_size = cur_data_value.key_size;
        cur_bucket.value_size = cur_data_value.value_size;
        cur_bucket.time_sec = cur_data_value.time_sec;
        cur_bucket.time_nsec = cur_data_value.time_nsec;
        cur_bucket.version = cur_data_value.version;
        cur_bucket.data_pos = cur_pos;
        cur_pos += 6*sizeof(uint32_t)+sizeof(uint64_t)+cur_data_value.key_size+cur_data_value.value_size;

        std::map<std::string, Bucket>::iterator it = index_.find(cur_data_value.key);
        if (it != index_.end())
        {
            bool is_new = false;
            Bucket *old_bucket = &(it->second);
            ret = CheckValueIsNew(cur_bucket, *old_bucket, &is_new);
            if (ret != base::kOk) return ret;
            if (is_new)
            {
                if (old_bucket->del_flag == kBCDelFlag && cur_bucket.del_flag == kBCExistFlag)
                    info_.used_cnt++;
                if (old_bucket->del_flag == kBCExistFlag && cur_bucket.del_flag == kBCDelFlag)
                    info_.used_cnt--;

                *old_bucket = cur_bucket;
            }
        }
        else
        {
            index_.insert(std::pair<std::string, Bucket>(cur_data_value.key, cur_bucket));
            if (cur_bucket.del_flag == kBCExistFlag) info_.used_cnt++;
        }
    }

    return ret;
}/*}}}*/

base::Code BitCaskDB::OpenNextFile(const std::string &prefix, uint64_t cur_suffix_num)
{/*{{{*/
    uint64_t next_suffix_num = cur_suffix_num+1;
    if (next_suffix_num >= kMaxFilesNum) return base::kFilesNumIsFull;

    std::string next_file_name;
    base::Code ret = GetFileName(prefix, next_suffix_num, &next_file_name);
    if (ret != base::kOk) return ret;

    ret = OpenAndReadFile(next_file_name, "a+"); 
    if (ret != base::kOk) return ret;

    return ret;
}/*}}}*/

base::Code BitCaskDB::CheckIsFileFull(const std::string &prefix, uint64_t cur_suffix_num, bool *full)
{/*{{{*/
    if (full == NULL) return base::kInvalidParam;
    *full = false;

    std::string cur_file_name;
    base::Code ret = GetFileName(prefix, cur_suffix_num, &cur_file_name);
    if (ret != base::kOk) return ret;
    std::string cur_file_path = dir_path_ + base::kSlashStr + cur_file_name;

    uint64_t cur_file_size = 0;
    ret = base::GetFileSize(cur_file_path, &cur_file_size);
    if (ret != base::kOk) return ret;
    if (cur_file_size >= kSingleFileSize) 
    {
        *full = true;
    }

    return ret;
}/*}}}*/

base::Code BitCaskDB::GetFileName(const std::string &prefix, uint64_t cur_suffix_num, std::string *file_name)
{/*{{{*/
    if (file_name == NULL) return base::kInvalidParam;

    char tmp_buf[base::kSmallBufLen] = {0};
    snprintf(tmp_buf, sizeof(tmp_buf)-1, "%c%017llu", base::kDot, (unsigned long long)cur_suffix_num);
    *file_name = prefix + tmp_buf; 

    return base::kOk;
}/*}}}*/

base::Code BitCaskDB::SetValue(const std::string &key, const std::string &value, int flag, int64_t version)
{/*{{{*/
    if (key.size() >= kMaxKeySize) return base::kKeySizeIsLarge;

    base::Code ret = base::kOk;
    DataValue cur_data_value;
    cur_data_value.Clear();
    std::map<std::string, Bucket>::iterator index_it = index_.find(key);
    Bucket *cur_bucket = NULL;
    if (index_it == index_.end())
    {
        if (flag == kBCDelFlag) return base::kNotFound;

        if (version != -1 && version != 0) return base::kCASFailed;
        cur_data_value.version = 1;
    }
    else 
    {
        cur_bucket = &(index_it->second);
        if (cur_bucket->del_flag == kBCDelFlag)
        {
            if (flag == kBCDelFlag) return base::kNotFound;

            if (version != -1 && version != 0) return base::kCASFailed;
            cur_data_value.version = 1;
        }
        else
        {
            if (version != -1 && version != (int64_t)cur_bucket->version) return base::kCASFailed;

            cur_data_value.version = cur_bucket->version + 1;
        }
    }

    cur_data_value.crc = base::CRC32(value.data(), (int)value.size());
    cur_data_value.del_flag = flag;

    ret = base::Time::GetTime(&(cur_data_value.time_sec), &(cur_data_value.time_nsec));
    if (ret != base::kOk) return ret;
    if (cur_bucket != NULL)
    {
        bool is_new = false;
        ret = CheckValueIsNew(cur_data_value.time_sec, cur_data_value.time_nsec, cur_bucket->time_sec, cur_bucket->time_nsec, &is_new);
        if (ret != base::kOk) return ret;
        if (!is_new) return base::kTimeWrong;
    }

    cur_data_value.key_size = key.size();
    cur_data_value.value_size = value.size();
    cur_data_value.key = key;
    cur_data_value.value = value;

    bool full = false;
    ret = CheckIsFileFull(kSourceDataFilePrefix, cur_data_file_suffix_num_, &full);
    if (ret != base::kOk) return ret;
    if (full)
    {
        ret = OpenNextFile(kSourceDataFilePrefix, cur_data_file_suffix_num_);
        if (ret != base::kOk) return ret;

        cur_data_file_suffix_num_++;
    }

    std::string cur_file_name;
    ret = GetFileName(kSourceDataFilePrefix, cur_data_file_suffix_num_, &cur_file_name);
    if (ret != base::kOk) return ret;
    std::map<std::string, FILE*>::iterator files_it = files_.find(cur_file_name);
    if (files_it == files_.end()) return base::kInvalidFileName;

    FILE *cur_fp = files_it->second;
    uint64_t cur_pos = 0;
    ret = WriteData(cur_data_value, &cur_pos, cur_fp);
    if (ret != base::kOk) return ret;

    Bucket new_bucket;
    new_bucket.fp = cur_fp;
    new_bucket.del_flag = cur_data_value.del_flag;
    new_bucket.key_size = cur_data_value.key_size;
    new_bucket.value_size = cur_data_value.value_size;
    new_bucket.data_pos = cur_pos;
    new_bucket.time_sec = cur_data_value.time_sec;
    new_bucket.time_nsec = cur_data_value.time_nsec;
    new_bucket.version = cur_data_value.version;

    index_it = index_.find(key);
    if (index_it == index_.end())
    {
        index_.insert(std::pair<std::string, Bucket>(key, new_bucket));

        if (flag == kBCExistFlag) ++info_.used_cnt;
    }
    else 
    {
        Bucket *cur_bucket = &(index_it->second);
        if (cur_bucket->del_flag == kBCDelFlag && flag == kBCExistFlag) ++info_.used_cnt;
        *cur_bucket = new_bucket;
    }

    return ret;
}/*}}}*/

base::Code BitCaskDB::ReadData(DataValue *data_value, FILE *fp)
{/*{{{*/
    if (fp == NULL || data_value == NULL) return base::kInvalidParam;
    
    if (ferror(fp)) return base::kReadError;
    if (feof(fp)) return base::kFileIsEnd;
    
    uint32_t head_len = 6*sizeof(uint32_t)+sizeof(uint64_t);
    char tmp_buf[base::kSmallBufLen] = {0};
    size_t r = fread(tmp_buf, sizeof(char), head_len, fp);
    if (r != head_len)
    {
        if (feof(fp)) return base::kFileIsEnd;
        return base::kReadError;
    }

    base::Code ret = DecodeDataValueHead(std::string(tmp_buf, head_len), data_value);
    if (ret != base::kOk) return ret;

    uint32_t data_len = data_value->key_size + data_value->value_size;
    r = fread(buf_, sizeof(char), data_len, fp);
    if (r != data_len) return base::kReadError;

    uint64_t start_pos = 0;
    data_value->key.assign(buf_+start_pos, data_value->key_size);
    
    start_pos += data_value->key_size;
    data_value->value.assign(buf_+start_pos,  data_value->value_size);

    uint32_t tmp_crc = base::CRC32(data_value->value.data(), data_value->value_size);
    if (tmp_crc != data_value->crc) return base::kDataValueError;

    return ret;
}/*}}}*/

base::Code BitCaskDB::WriteData(const DataValue &data_value, uint64_t *cur_pos, FILE *fp)
{/*{{{*/
    if (cur_pos == NULL || fp == NULL) return base::kInvalidParam;

    if (ferror(fp)) return base::kReadError;

    std::string dump_str;
    base::Code ret = EncodeDataValue(data_value, &dump_str);
    if (ret != base::kOk) return ret;

    int data_fd = fileno(fp);
    ret = base::GetFileSize(data_fd, cur_pos);
    if (ret != base::kOk) return ret;

    int r = fwrite(dump_str.data(), sizeof(char), dump_str.size(), fp);
    if (r != (int)dump_str.size()) return base::kWriteError;

    fflush(fp);
    fsync(data_fd);

    return ret;
}/*}}}*/

base::Code BitCaskDB::CheckValueIsNew(const Bucket &first_bucket, const Bucket &second_bucket, bool *is_new)
{/*{{{*/
    if (is_new == NULL) return base::kInvalidParam;

    *is_new = false;
    if (first_bucket.time_sec > second_bucket.time_sec ||
        (first_bucket.time_sec == second_bucket.time_sec && first_bucket.time_nsec > second_bucket.time_nsec))
    {
        *is_new = true;
    }
        
    return base::kOk;
}/*}}}*/

base::Code BitCaskDB::CheckValueIsNew(uint32_t first_sec, uint32_t first_nsec, uint32_t second_sec, uint32_t second_nsec, bool *is_new)
{/*{{{*/
    if (is_new == NULL) return base::kInvalidParam;

    *is_new = false;
    if (first_sec > second_sec ||
        (first_sec == second_sec && first_nsec > second_nsec))
    {
        *is_new = true;
    }
        
    return base::kOk;
}/*}}}*/

base::Code BitCaskDB::EncodeDataValueHead(const DataValue &data_value, std::string *str)
{/*{{{*/
    if (str == NULL) return base::kInvalidParam;

    str->clear();
   
    std::string tmp_str;
    base::Code ret = base::EncodeFixed32(data_value.crc, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.del_flag, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.time_sec, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.time_nsec, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed64(data_value.version, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.key_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.value_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    return ret;
}/*}}}*/

base::Code BitCaskDB::DecodeDataValueHead(const std::string &str, DataValue *data_value)
{/*{{{*/
    if (data_value == NULL) return base::kInvalidParam;

    uint32_t tmp_num = 0;
    uint64_t tmp64_num = 0;
    uint64_t start_pos = 0;

    base::Code ret = base::DecodeFixed32(std::string(str, 0, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->crc = tmp_num;
    start_pos += sizeof(uint32_t);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->del_flag = tmp_num;
    start_pos += sizeof(uint32_t);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->time_sec = tmp_num;
    start_pos += sizeof(uint32_t);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->time_nsec = tmp_num;
    start_pos += sizeof(uint32_t);

    tmp64_num = 0;
    ret = base::DecodeFixed64(std::string(str, start_pos, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    data_value->version = tmp64_num;
    start_pos += sizeof(uint64_t);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->key_size = tmp_num;
    start_pos += sizeof(uint32_t);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->value_size = tmp_num;

    return ret;
}/*}}}*/

base::Code BitCaskDB::EncodeDataValue(const DataValue &data_value, std::string *str)
{/*{{{*/
    if (str == NULL) return base::kInvalidParam;

    str->clear();
    base::Code ret = EncodeDataValueHead(data_value, str);
    if (ret != base::kOk) return ret;

    str->append(data_value.key);
    str->append(data_value.value);

    return ret;
}/*}}}*/

base::Code BitCaskDB::DecodeDataValue(const std::string &str, DataValue *data_value)
{/*{{{*/
    if (data_value == NULL) return base::kInvalidParam;

    uint64_t start_pos = 0;

    base::Code ret = DecodeDataValueHead(str, data_value);
    if (ret != base::kOk) return ret;

    start_pos = 6*sizeof(uint32_t) + sizeof(uint64_t);
    data_value->key.assign(str, start_pos, data_value->key_size);

    start_pos += data_value->key_size;
    data_value->value.assign(str, start_pos, data_value->value_size);

    return ret;
}/*}}}*/


}
