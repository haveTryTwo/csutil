// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "errno.h"
#include "assert.h"

#include "base/log.h"
#include "base/hash.h"
#include "base/coding.h"
#include "base/file_util.h"

#include "hash_db.h"

namespace store
{

HashDB::HashDB() : path_(""), index_fp_(NULL), data_fp_(NULL), index_(NULL)
{/*{{{*/
    buf_ = new char[kMaxDataSize];
    memset(buf_, 0, sizeof(kMaxDataSize));
}/*}}}*/

HashDB::~HashDB()
{/*{{{*/
    Destroy();

    if (buf_ != NULL)
    {
        delete []buf_;
        buf_ = NULL;
    }
}/*}}}*/

base::Code HashDB::Put(const std::string &key, const std::string &value, int64_t version/*=-1*/)
{/*{{{*/
    if (value.size() >= kMaxDataSize) return base::kValueSizeIsLarger;
//    if (index_->info.max_num >= kMaxKeysNum) return base::kDBFull;

    base::Code ret = base::kOk;
    Bucket bkt;
    DataValue data_value;
    uint64_t hash_pos = 0; // hash position in hash_dir
    uint64_t next_pos = 0; // bucket position in bucket
    uint64_t cur_pos = 0;
    bool is_exist = false;
    char tmp_buf64[sizeof(uint64_t)] = {0};
    char tmp_buf_bkt[sizeof(Bucket)] = {0};
    
    uint32_t crc_key = base::CRC32(key.data(), key.size());
    hash_pos = (crc_key % (sizeof(index_->hash_dir)/sizeof(index_->hash_dir[0])))*sizeof(uint64_t) + sizeof(Info);

    fseek(index_fp_, hash_pos, SEEK_SET);
    int r = fread(tmp_buf64, sizeof(char), sizeof(uint64_t), index_fp_);
    if (r != sizeof(uint64_t))
    {
        base::LOG_ERR("Failed to hash_pos:%llu in index file, ret:%d, errno:%d",
                (unsigned long long)hash_pos, r, errno);
        return base::kReadError;
    }

    ret = base::DecodeFixed64(std::string(tmp_buf64, sizeof(uint64_t)), &next_pos);
    if (ret != base::kOk) return ret;

    while (next_pos != 0)
    {/*{{{*/
        cur_pos = next_pos;

        memset(&bkt, 0, sizeof(Bucket));
        memset(tmp_buf_bkt, 0, sizeof(Bucket));

        fseek(index_fp_, next_pos, SEEK_SET);
        r = fread(tmp_buf_bkt, sizeof(char), sizeof(Bucket), index_fp_);
        if (r != sizeof(Bucket))
        {
            base::LOG_ERR("Failed to bucket_pos:%llu in index file, ret:%d, errno:%d",
                    (unsigned long long)next_pos, r, errno);
            return base::kReadError;
        }

        ret = DecodeBucket(std::string(tmp_buf_bkt, sizeof(tmp_buf_bkt)), &bkt);
        if (ret != base::kOk) return ret;

        if (crc_key == bkt.key_hash && key.size() == bkt.key_size && 
            memcmp(key.data(), bkt.pre_key, 
                   sizeof(bkt.pre_key)<key.size()?sizeof(bkt.pre_key):key.size()) == 0)
        {/*{{{*/
            uint32_t data_len = 4*sizeof(uint32_t) + bkt.key_size + bkt.value_size + sizeof(uint64_t);
            fseek(data_fp_, bkt.data_pos, SEEK_SET);
            r = fread(buf_, sizeof(char), data_len, data_fp_);
            if (r != data_len)
            {
                base::LOG_ERR("Failed to read data_pos:%llu in data file, ret:%d, errno:%d",
                        (unsigned long long)bkt.data_pos, r, errno);
                return base::kReadError;
            }

            data_value.Clear();
            ret = DecodeDataValue(std::string(buf_, data_len), &data_value);
            if (ret != base::kOk) return ret;
            
            uint32_t tmp_value_crc = base::CRC32(data_value.value.data(), data_value.value_size);
            if (data_value.crc != tmp_value_crc)
            {
                base::LOG_ERR("Failed to check data value crc");
                return base::kDataValueError;
            }

            assert(bkt.key_size == data_value.key_size);
            if (memcmp(key.data(), data_value.key.data(), key.size()) == 0)
            {
                is_exist = true;
                break;
            }
        }/*}}}*/

        next_pos = bkt.next_pos;
    }/*}}}*/

    if (is_exist && version != -1 && version != (int64_t)bkt.version)
    {
        return base::kCASFailed;
    }
    
    if (!is_exist && version != -1 && version != 0)
    {
        return base::kCASFailed;
    }

    uint64_t data_file_size = 0;
    ret = base::GetFileSize(path_+".dat", &data_file_size);
    if (ret != base::kOk) return ret;
    data_value.Clear();
    data_value.key = key;
    data_value.value = value;
    data_value.key_size = key.size();
    data_value.value_size = value.size();
    data_value.total_size = sizeof(uint32_t)*2 + data_value.key_size + data_value.value_size;

    if (is_exist)
    {/*{{{*/
        bkt.version++;
        bkt.value_size = data_value.value_size;
        bkt.data_pos = data_file_size;

        // write new data
        data_value.version = bkt.version;
        data_value.crc = base::CRC32(data_value.value.data(), data_value.value_size);
        std::string tmp_data_value;
        ret = EncodeDataValue(data_value, &tmp_data_value);
        if (ret != base::kOk) return ret;

        fseek(data_fp_, data_file_size, SEEK_SET);
        int r = fwrite(tmp_data_value.data(), sizeof(char), tmp_data_value.size(), data_fp_);
        if (r != (int)tmp_data_value.size()) return base::kWriteError;

        // update bucket info
        std::string tmp_bkt_value;
        ret = EncodeBucket(bkt, &tmp_bkt_value);
        if (ret != base::kOk) return ret;

        fseek(index_fp_, cur_pos, SEEK_SET);
        r = fwrite(tmp_bkt_value.data(), sizeof(char), tmp_bkt_value.size(), index_fp_);
        if (r != (int)tmp_bkt_value.size()) return base::kWriteError;
    }/*}}}*/
    else
    {/*{{{*/
        uint64_t hash_file_size = 0;
        ret = base::GetFileSize(path_+".idx", &hash_file_size);
        if (ret != base::kOk) return ret;

        Bucket new_bkt;
        new_bkt.Clear();
        new_bkt.version = 1;
        new_bkt.next_pos = 0;
        new_bkt.key_hash = crc_key;
        memcpy(new_bkt.pre_key, key.data(), sizeof(new_bkt.pre_key)<key.size()?
                                            sizeof(new_bkt.pre_key):key.size());
        new_bkt.key_size = key.size();
        new_bkt.value_size = value.size();
        new_bkt.data_pos = data_file_size;

        // Write new data
        data_value.version = new_bkt.version;
        data_value.crc = base::CRC32(data_value.value.data(), data_value.value_size);
        std::string tmp_data_value;
        ret = EncodeDataValue(data_value, &tmp_data_value);
        if (ret != base::kOk) return ret;

        fseek(data_fp_, data_file_size, SEEK_SET);
        int r = fwrite(tmp_data_value.data(), sizeof(char), tmp_data_value.size(), data_fp_);
        if (r != (int)tmp_data_value.size()) return base::kWriteError;
 
        // Set new bucket info
        std::string tmp_bkt_value;
        ret = EncodeBucket(new_bkt, &tmp_bkt_value);
        if (ret != base::kOk) return ret;

        fseek(index_fp_, hash_file_size, SEEK_SET);
        r = fwrite(tmp_bkt_value.data(), sizeof(char), tmp_bkt_value.size(), index_fp_);
        if (r != (int)tmp_bkt_value.size()) return base::kWriteError;
        
        // Update prefix bucket info
        std::string tmp_hash_file_size;
        ret = base::EncodeFixed64(hash_file_size, &tmp_hash_file_size);
        if (ret != base::kOk) return ret;

        if (cur_pos == 0)
        {
            fseek(index_fp_, hash_pos, SEEK_SET);
            r = fwrite(tmp_hash_file_size.data(), sizeof(char), tmp_hash_file_size.size(), index_fp_);
            if (r != (int)tmp_hash_file_size.size()) return base::kWriteError;
        }
        else
        {
            bkt.next_pos = hash_file_size;

            // Update prefix bucket info
            tmp_bkt_value.clear();
            ret = EncodeBucket(bkt, &tmp_bkt_value);
            if (ret != base::kOk) return ret;

            fseek(index_fp_, cur_pos, SEEK_SET);
            r = fwrite(tmp_bkt_value.data(), sizeof(char), tmp_bkt_value.size(), index_fp_);
            if (r != (int)tmp_bkt_value.size()) return base::kWriteError;
        }
    }/*}}}*/

    if (!is_exist) 
    {
        index_->info.used_cnt++;
    }
    index_->info.trx_id++;
    std::string tmp_info;
    ret = EncodeInfo(index_->info, &tmp_info);
    if (ret != base::kOk) return ret;

    // Update info
    fseek(index_fp_, 0, SEEK_SET);
    r = fwrite(tmp_info.data(), sizeof(char), tmp_info.size(), index_fp_);
    if (r != (int)tmp_info.size()) return base::kWriteError;

    fflush(data_fp_);
    fflush(index_fp_);

    return base::kOk;
}/*}}}*/

base::Code HashDB::Get(const std::string &key, std::string *value, int64_t *version/*=NULL*/)
{/*{{{*/
    if (value == NULL) return base::kInvalidParam;

    base::Code ret = base::kOk;
    Bucket bkt;
    uint64_t hash_pos = 0; // hash position in hash_dir
    uint64_t next_pos = 0; // bucket position in bucket
    char tmp_buf64[sizeof(uint64_t)] = {0};
    char tmp_buf_bkt[sizeof(Bucket)] = {0};
    
    uint32_t crc_key = base::CRC32(key.data(), key.size());
    hash_pos = (crc_key % (sizeof(index_->hash_dir)/sizeof(index_->hash_dir[0])))*sizeof(uint64_t) + sizeof(Info);

    fseek(index_fp_, hash_pos, SEEK_SET);
    int r = fread(tmp_buf64, sizeof(char), sizeof(uint64_t), index_fp_);
    if (r != sizeof(uint64_t))
    {
        base::LOG_ERR("Failed to hash_pos:%llu in index file, ret:%d, errno:%d",
                (unsigned long long)hash_pos, r, errno);
        return base::kReadError;
    }

    ret = base::DecodeFixed64(std::string(tmp_buf64, sizeof(uint64_t)), &next_pos);
    if (ret != base::kOk) return ret;
    if (next_pos == 0) return base::kNotFound; // There is no such key

    DataValue data_value;
    while (next_pos != 0)
    {/*{{{*/
        memset(&bkt, 0, sizeof(Bucket));
        memset(tmp_buf_bkt, 0, sizeof(Bucket));

        fseek(index_fp_, next_pos, SEEK_SET);
        r = fread(tmp_buf_bkt, sizeof(char), sizeof(Bucket), index_fp_);
        if (r != sizeof(Bucket))
        {
            base::LOG_ERR("Failed to read data at_pos:%llu in index file, ret:%d, expect len:%zd, errno:%d",
                    (unsigned long long)next_pos, r, sizeof(Bucket), errno);
            return base::kReadError;
        }

        ret = DecodeBucket(std::string(tmp_buf_bkt, sizeof(tmp_buf_bkt)), &bkt);
        if (ret != base::kOk) return ret;

        if (crc_key == bkt.key_hash && key.size() == bkt.key_size && 
            memcmp(key.data(), bkt.pre_key, 
                   sizeof(bkt.pre_key)<key.size()?sizeof(bkt.pre_key):key.size()) == 0)
        {/*{{{*/
            uint32_t data_len = 4*sizeof(uint32_t) + bkt.key_size + bkt.value_size + sizeof(uint64_t);
            fseek(data_fp_, bkt.data_pos, SEEK_SET);
            r = fread(buf_, sizeof(char), data_len, data_fp_);
            if (r != data_len)
            {
                base::LOG_ERR("Failed to read data_pos:%llu in data file, ret:%d, errno:%d",
                        (unsigned long long)bkt.data_pos, r, errno);
                return base::kReadError;
            }

            data_value.Clear();
            ret = DecodeDataValue(std::string(buf_, data_len), &data_value);
            if (ret != base::kOk) return ret;
            
            uint32_t tmp_value_crc = base::CRC32(data_value.value.data(), data_value.value_size);
            if (data_value.crc != tmp_value_crc)
            {
                base::LOG_ERR("Failed to check data value crc");
                return base::kDataValueError;
            }

            assert(bkt.key_size == data_value.key_size);
            if (memcmp(key.data(), data_value.key.data(), key.size()) == 0)
            {
                *value = data_value.value;
                if (version != NULL) *version = (int64_t)data_value.version;

                return base::kOk;
            }
        }/*}}}*/

        next_pos = bkt.next_pos;
    }/*}}}*/

    return base::kNotFound;
}/*}}}*/

base::Code HashDB::Del(const std::string &key, int64_t version/*=-1*/)
{/*{{{*/
    base::Code ret = base::kOk;
    Bucket bkt;
    Bucket pre_bkt;
    DataValue data_value;
    uint64_t hash_pos = 0; // hash position in hash_dir
    uint64_t next_pos = 0; // bucket position in bucket
    uint64_t cur_pos = 0;
    uint64_t pre_pos = 0;
    bool is_exist = false;
    char tmp_buf64[sizeof(uint64_t)] = {0};
    char tmp_buf_bkt[sizeof(Bucket)] = {0};
    
    uint32_t crc_key = base::CRC32(key.data(), key.size());
    hash_pos = (crc_key % (sizeof(index_->hash_dir)/sizeof(index_->hash_dir[0])))*sizeof(uint64_t) + sizeof(Info);

    fseek(index_fp_, hash_pos, SEEK_SET);
    int r = fread(tmp_buf64, sizeof(char), sizeof(uint64_t), index_fp_);
    if (r != sizeof(uint64_t))
    {
        base::LOG_ERR("Failed to hash_pos:%llu in index file, ret:%d, errno:%d",
                (unsigned long long)hash_pos, r, errno);
        return base::kReadError;
    }

    ret = base::DecodeFixed64(std::string(tmp_buf64, sizeof(uint64_t)), &next_pos);
    if (ret != base::kOk) return ret;

    bkt.Clear();
    while (next_pos != 0)
    {/*{{{*/
        pre_pos = cur_pos;
        pre_bkt = bkt;

        cur_pos = next_pos;

        memset(&bkt, 0, sizeof(Bucket));
        memset(tmp_buf_bkt, 0, sizeof(Bucket));

        fseek(index_fp_, next_pos, SEEK_SET);
        r = fread(tmp_buf_bkt, sizeof(char), sizeof(Bucket), index_fp_);
        if (r != sizeof(Bucket))
        {
            base::LOG_ERR("Failed to bucket_pos:%llu in index file, ret:%d, errno:%d",
                    (unsigned long long)next_pos, r, errno);
            return base::kReadError;
        }

        ret = DecodeBucket(std::string(tmp_buf_bkt, sizeof(tmp_buf_bkt)), &bkt);
        if (ret != base::kOk) return ret;

        if (crc_key == bkt.key_hash && key.size() == bkt.key_size && 
            memcmp(key.data(), bkt.pre_key, 
                   sizeof(bkt.pre_key)<key.size()?sizeof(bkt.pre_key):key.size()) == 0)
        {/*{{{*/
            uint32_t data_len = 4*sizeof(uint32_t) + bkt.key_size + bkt.value_size + sizeof(uint64_t);
            fseek(data_fp_, bkt.data_pos, SEEK_SET);
            r = fread(buf_, sizeof(char), data_len, data_fp_);
            if (r != data_len)
            {
                base::LOG_ERR("Failed to read data_pos:%llu in data file, ret:%d, errno:%d",
                        (unsigned long long)bkt.data_pos, r, errno);
                return base::kReadError;
            }

            data_value.Clear();
            ret = DecodeDataValue(std::string(buf_, data_len), &data_value);
            if (ret != base::kOk) return ret;
            
            uint32_t tmp_value_crc = base::CRC32(data_value.value.data(), data_value.value_size);
            if (data_value.crc != tmp_value_crc)
            {
                base::LOG_ERR("Failed to check data value crc");
                return base::kDataValueError;
            }

            assert(bkt.key_size == data_value.key_size);
            if (memcmp(key.data(), data_value.key.data(), key.size()) == 0)
            {
                is_exist = true;
                break;
            }
        }/*}}}*/

        next_pos = bkt.next_pos;
    }/*}}}*/

    if (!is_exist) return base::kNotFound;

    if (is_exist && version != -1 && version != (int64_t)bkt.version)
    {
        return base::kCASFailed;
    }

    next_pos = bkt.next_pos;

    if (pre_pos == 0)
    {
        // Update prefix bucket info
        std::string tmp_hash_file_size;
        ret = base::EncodeFixed64(next_pos, &tmp_hash_file_size);
        if (ret != base::kOk) return ret;

        fseek(index_fp_, hash_pos, SEEK_SET);
        r = fwrite(tmp_hash_file_size.data(), sizeof(char), tmp_hash_file_size.size(), index_fp_);
        if (r != (int)tmp_hash_file_size.size()) return base::kWriteError;
    }
    else
    {
        pre_bkt.next_pos = next_pos;

        // Update prefix bucket info
        std::string tmp_bkt_value;
        ret = EncodeBucket(pre_bkt, &tmp_bkt_value);
        if (ret != base::kOk) return ret;

        fseek(index_fp_, pre_pos, SEEK_SET);
        r = fwrite(tmp_bkt_value.data(), sizeof(char), tmp_bkt_value.size(), index_fp_);
        if (r != (int)tmp_bkt_value.size()) return base::kWriteError;
    }

    index_->info.used_cnt--;
    index_->info.trx_id++;
    std::string tmp_info;
    ret = EncodeInfo(index_->info, &tmp_info);
    if (ret != base::kOk) return ret;

    // Update info
    fseek(index_fp_, 0, SEEK_SET);
    r = fwrite(tmp_info.data(), sizeof(char), tmp_info.size(), index_fp_);
    if (r != (int)tmp_info.size()) return base::kWriteError;

    fflush(data_fp_);
    fflush(index_fp_);

    return base::kOk;
}/*}}}*/

base::Code HashDB::Init(const std::string &path)
{/*{{{*/
    if (path.empty()) return base::kInvalidParam;

    path_ = path;
    base::Code ret = base::kOk;

    std::string index_path = path_ + ".idx";
    std::string data_path = path_ + ".dat";
    std::string op_type;
    bool is_first = false;

    index_ = new Index();
    memset(index_, 0, sizeof(Index));

    bool is_exist = false;
    ret = base::CheckFileExist(index_path, &is_exist);
    if (ret != base::kOk) return ret;

    if (is_exist)
    {
        op_type = "r+";
    }
    else
    {
        is_first = true;
        op_type = "w+";
    }

    // Open index file
    index_fp_ = fopen(index_path.c_str(), op_type.c_str());
    if (index_fp_ == NULL)
    {
        base::LOG_ERR("Failed to open index file:%s, errno:%d", index_path.c_str(), errno);
        ret = base::kOpenFileFailed;
        goto index_err;
    }
    
    ret = InitIndex(is_first);
    if (ret != base::kOk)
    {
        base::LOG_ERR("Failed to init index, ret:%d", ret);
        goto index_err;
    }

    // Open data file
    ret = base::CheckFileExist(data_path, &is_exist);
    if (ret != base::kOk) return ret;
    if (is_exist)
    {
        op_type = "r+";
    }
    else
    {
        op_type = "w+";
    }

    data_fp_ = fopen(data_path.c_str(), op_type.c_str());
    if (data_fp_ == NULL)
    {
        base::LOG_ERR("Failed to open data file:%s, errno:%d", data_path.c_str(), errno);
        ret = base::kOpenFileFailed;
        goto data_err;
    }

    return base::kOk;

data_err:
    Destroy();
index_err:
    return ret;
}/*}}}*/

base::Code HashDB::GetStatus(DBStatusInfo *status_info)
{/*{{{*/
    if (status_info == NULL) return base::kInvalidParam;

    status_info->max_num = index_->info.max_num;
    status_info->used_cnt = index_->info.used_cnt;
    status_info->trx_id = index_->info.trx_id;

    Bucket bkt;
    uint64_t next_pos = 0;
    base::Code ret = base::kOk;
    char tmp_buf64[sizeof(uint64_t)] = {0};
    char tmp_buf_bkt[sizeof(Bucket)] = {0};

    for (uint32_t i = 0; i < sizeof(index_->hash_dir)/sizeof(index_->hash_dir[0]); ++i)
    {/*{{{*/
        uint64_t hash_pos = i*sizeof(uint64_t) + sizeof(Info);

        fseek(index_fp_, hash_pos, SEEK_SET);
        int r = fread(tmp_buf64, sizeof(char), sizeof(uint64_t), index_fp_);
        if (r != sizeof(uint64_t))
        {
            base::LOG_ERR("Failed to hash_pos:%llu in index file, ret:%d, errno:%d",
                    (unsigned long long)hash_pos, r, errno);
            return base::kReadError;
        }

        ret = base::DecodeFixed64(std::string(tmp_buf64, sizeof(uint64_t)), &next_pos);
        if (ret != base::kOk) return ret;

        uint32_t same_hash_num = 0;
        while (next_pos != 0)
        {/*{{{*/
            ++same_hash_num;
            bkt.Clear();
            memset(tmp_buf_bkt, 0, sizeof(Bucket));

            fseek(index_fp_, next_pos, SEEK_SET);
            r = fread(tmp_buf_bkt, sizeof(char), sizeof(Bucket), index_fp_);
            if (r != sizeof(Bucket))
            {
                base::LOG_ERR("Failed to bucket_pos:%llu in index file, ret:%d, errno:%d",
                        (unsigned long long)next_pos, r, errno);
                return base::kReadError;
            }

            ret = DecodeBucket(std::string(tmp_buf_bkt, sizeof(tmp_buf_bkt)), &bkt);
            if (ret != base::kOk) return ret;

            next_pos = bkt.next_pos;
        }/*}}}*/

        std::map<uint32_t, uint32_t>::iterator it = status_info->same_hash_status.find(same_hash_num);
        if (it == status_info->same_hash_status.end())
        {
            status_info->same_hash_status.insert(std::make_pair<uint32_t, uint32_t>(same_hash_num, 1));
        }
        else
        {
            it->second++;
            status_info->same_hash_status[it->first] = it->second;
        }
    }/*}}}*/

    return base::kOk;
}/*}}}*/

base::Code HashDB::Destroy()
{/*{{{*/
    if (index_fp_ != NULL)
    {
        fclose(index_fp_);
        index_fp_ = NULL;
    }

    if (data_fp_ != NULL)
    {
        fclose(data_fp_);
        data_fp_ = NULL;
    }

    if (index_ != NULL)
    {
        delete index_;
        index_ = NULL;
    }

    return base::kOk;
}/*}}}*/

base::Code HashDB::InitIndex(bool is_first)
{/*{{{*/
    base::Code ret = base::kOtherFailed;

    if (is_first)
    {/*{{{*/
        index_->info.max_num = kMaxKeysNum;
        index_->info.used_cnt = 0;
        index_->info.trx_id = 0;

        std::string info_tmp_str;
        base::Code ret = EncodeInfo(index_->info, &info_tmp_str);
        if (ret != base::kOk) return ret;

        int r = fwrite(info_tmp_str.data(), sizeof(char), info_tmp_str.size(), index_fp_);
        if (r != info_tmp_str.size())
        {
            base::LOG_ERR("Failed to write info to index file!");
            return base::kWriteError;
        }

        r = fwrite(index_->hash_dir, sizeof(uint64_t), kMaxKeysNum, index_fp_);
        if (r != kMaxKeysNum)
        {
            base::LOG_ERR("Failed to write hash_dir to index file!");
            return base::kWriteError;
        }

        fflush(index_fp_);
        
        return base::kOk;
    }/*}}}*/
    else
    {/*{{{*/
        char buf[sizeof(Info)] = {0};
        int r = fread(buf, sizeof(char), sizeof(buf), index_fp_);
        if (r != sizeof(buf))
        {
            base::LOG_ERR("Failed to read info from index file!");
            return base::kReadError;
        }
        base::Code ret = DecodeInfo(std::string(buf, sizeof(buf)), &(index_->info));
        if (ret != base::kOk) return ret;

        assert(index_->info.max_num > 0);

        return base::kOk;
    }/*}}}*/

    return ret;
}/*}}}*/

base::Code HashDB::EncodeInfo(const Info &info, std::string *str)
{/*{{{*/
    if (str == NULL) return base::kInvalidParam;

    str->clear();
   
    std::string tmp_str;
    base::Code ret = base::EncodeFixed32(info.max_num, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(info.used_cnt, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed64(info.trx_id, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    return ret;
}/*}}}*/

base::Code HashDB::DecodeInfo(const std::string &str, Info *info)
{/*{{{*/
    if (info == NULL) return base::kInvalidParam;

    uint32_t tmp_num = 0;
    base::Code ret = base::DecodeFixed32(std::string(str, 0, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    info->max_num = tmp_num;

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, 4, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    info->used_cnt = tmp_num;

    uint64_t tmp64_num = 0;
    ret = base::DecodeFixed64(std::string(str, 8, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    info->trx_id = tmp64_num;

    return ret;
}/*}}}*/

base::Code HashDB::EncodeBucket(const Bucket &bucket, std::string *str)
{/*{{{*/
    if (str == NULL) return base::kInvalidParam;

    str->clear();
   
    std::string tmp_str;
    base::Code ret = base::EncodeFixed64(bucket.version, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed64(bucket.next_pos, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(bucket.key_hash, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    str->append(bucket.pre_key, sizeof(bucket.pre_key));

    tmp_str.clear();
    ret = base::EncodeFixed32(bucket.key_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(bucket.value_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed64(bucket.data_pos, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    return ret;
}/*}}}*/

base::Code HashDB::DecodeBucket(const std::string &str, Bucket *bucket)
{/*{{{*/
    if (bucket == NULL) return base::kInvalidParam;

    uint32_t tmp_num = 0;
    uint64_t tmp64_num = 0;
    uint32_t start_pos = 0;

    base::Code ret = base::DecodeFixed64(std::string(str, start_pos, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    bucket->version = tmp64_num;
    start_pos += 8;

    tmp64_num = 0;
    ret = base::DecodeFixed64(std::string(str, start_pos, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    bucket->next_pos = tmp64_num;
    start_pos += 8;

    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    bucket->key_hash = tmp_num;
    start_pos += 4;

    memcpy(bucket->pre_key, str.data()+start_pos, sizeof(bucket->pre_key));
    start_pos += sizeof(bucket->pre_key);

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    bucket->key_size = tmp_num;
    start_pos += 4;

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    bucket->value_size = tmp_num;
    start_pos += 4;

    tmp64_num = 0;
    ret = base::DecodeFixed64(std::string(str, start_pos, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    bucket->data_pos = tmp64_num;

    return ret;
}/*}}}*/

base::Code HashDB::EncodeDataValue(const DataValue &data_value, std::string *str)
{/*{{{*/
    if (str == NULL) return base::kInvalidParam;

    str->clear();
   
    std::string tmp_str;
    base::Code ret = base::EncodeFixed32(data_value.total_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.key_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    str->append(data_value.key);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.value_size, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    str->append(data_value.value);

    tmp_str.clear();
    ret = base::EncodeFixed64(data_value.version, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    tmp_str.clear();
    ret = base::EncodeFixed32(data_value.crc, &tmp_str);
    if (ret != base::kOk) return ret;
    str->append(tmp_str);

    return ret;
}/*}}}*/

base::Code HashDB::DecodeDataValue(const std::string &str, DataValue *data_value)
{/*{{{*/
    if (data_value == NULL) return base::kInvalidParam;

    uint32_t tmp_num = 0;
    uint64_t tmp64_num = 0;
    uint64_t start_pos = 0;

    base::Code ret = base::DecodeFixed32(std::string(str, 0, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->total_size = tmp_num;
    start_pos += 4;

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->key_size = tmp_num;
    start_pos += 4;

    data_value->key.assign(str, start_pos, data_value->key_size);
    start_pos += data_value->key_size;

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->value_size = tmp_num;
    start_pos += 4;

    data_value->value.assign(str, start_pos, data_value->value_size);
    start_pos += data_value->value_size;

    tmp64_num = 0;
    ret = base::DecodeFixed64(std::string(str, start_pos, sizeof(uint64_t)), &tmp64_num);
    if (ret != base::kOk) return ret;
    data_value->version = tmp64_num;
    start_pos += 8;

    tmp_num = 0;
    ret = base::DecodeFixed32(std::string(str, start_pos, sizeof(uint32_t)), &tmp_num);
    if (ret != base::kOk) return ret;
    data_value->crc = tmp_num;

    return ret;
}/*}}}*/


}
