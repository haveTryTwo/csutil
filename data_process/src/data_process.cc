// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <utility>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <assert.h>

#include "base/log.h"
#include "base/int.h"
#include "base/hash.h"
#include "base/util.h"
#include "base/common.h"
#include "base/file_util.h"

#include "data_process.h"

namespace base
{
DataProcess::DataProcess(char delim, int num_of_columns, bool check_columns_flag, int num_of_sorting_key, int first_key_index, int second_key_index, int num_of_hash_files) :
    delim_(delim),
    num_of_columns_(num_of_columns), 
    check_num_of_columns_flag_(check_columns_flag),
    num_of_sorting_key_(num_of_sorting_key),
    first_key_index_(first_key_index),
    second_key_index_(second_key_index),
    num_of_hash_files_(num_of_hash_files)
{/*{{{*/
    way_of_reading_file_ = kLine;
    way_of_writing_file_ = kLine;
    first_key_sorting_ = kAscend;
    second_key_sorting_ = kAscend;
    way_of_hash_ = kBKDRHash;
}/*}}}*/

DataProcess::~DataProcess()
{
}

Code DataProcess::SetDelim(char delim)
{/*{{{*/
    delim_ = delim;
    return kOk;
}/*}}}*/

Code DataProcess::SetNumOfColumns(int num_of_columns)
{/*{{{*/
    num_of_columns_ = num_of_columns;
    return kOk;
}/*}}}*/

Code DataProcess::SetCheckCloumnFlag(bool check_columns_flag)
{/*{{{*/
    check_num_of_columns_flag_ = check_columns_flag;
    return kOk;
}/*}}}*/

Code DataProcess::SetReadWay(OpFileWay read_way)
{/*{{{*/
    way_of_reading_file_ = read_way;
    return kOk;
}/*}}}*/

Code DataProcess::SetWriteWay(OpFileWay write_way)
{/*{{{*/
    way_of_writing_file_ = write_way;
    return kOk;
}/*}}}*/

Code DataProcess::SetNumOfSortingKey(int num_of_sorting_key)
{/*{{{*/
    num_of_sorting_key_ = num_of_sorting_key;
    return kOk;
}/*}}}*/

Code DataProcess::SetFirstKeyIndex(int first_key_index)
{/*{{{*/
    first_key_index_ = first_key_index;
    return kOk;
}/*}}}*/

Code DataProcess::SetFirstKeySortWay(SortWay first_key_sort_way)
{/*{{{*/
    first_key_sorting_ = first_key_sort_way;
    return kOk;
}/*}}}*/

Code DataProcess::SetSecondKeyIndex(int second_key_index)
{/*{{{*/
    second_key_index_ = second_key_index;
    return kOk;
}/*}}}*/

Code DataProcess::SetSecondKeySortWay(SortWay second_key_sort_way)
{/*{{{*/
    second_key_sorting_ = second_key_sort_way;
    return kOk;
}/*}}}*/

Code DataProcess::SetNumOfHashFiles(int num_of_hash_files)
{/*{{{*/
    num_of_hash_files_ = num_of_hash_files;
    return kOk;
}/*}}}*/

Code DataProcess::SetHashWay(HashWay hash_way)
{/*{{{*/
    way_of_hash_ = hash_way;
    return kOk;
}/*}}}*/


Code DataProcess::ReadData(std::string *content, FILE *fp)
{/*{{{*/
    if (content == NULL || fp == NULL) return kInvalidParam;

    Code ret = kOk;
    switch (way_of_reading_file_)
    {
        case kLine:
            ret = PumpStringData(content, fp);
            break;
        case kLenAndContent:
            ret = PumpBinData(content, fp);
            break;
        default:
            LOG_ERR("Invalid way of reading file:%d", way_of_reading_file_);
            ret = kInvalidWayOfReadingFile;
            break;
    }
    return ret;
}/*}}}*/

Code DataProcess::WriteData(const std::string &content, FILE *fp)
{/*{{{*/
    if (content.empty() || fp == NULL) return kInvalidParam;

    Code ret = kOk;
    switch (way_of_writing_file_)
    {
        case kLine:
            ret = DumpStringData(content, fp);
            break;
        case kLenAndContent:
            ret = DumpBinData(content, fp);
            break;
        default:
            LOG_ERR("Invalid way of writing file:%d", way_of_writing_file_);
            ret = kInvalidWayOfWritingFile;
            break;
    }
    return ret;
}/*}}}*/

Code DataProcess::WriteStat(const std::string &key, int64_t file_pos, int count, char stat_delim, FILE *fp)
{/*{{{*/
    if (key.empty() || fp == NULL) return kInvalidParam;

    char buf[kBufLen] = {0};
    snprintf(buf, sizeof(buf), "%c%lld%c%d\n", stat_delim, file_pos, stat_delim, count);
    std::string value = key + buf;
    Code ret = DumpStringData(value, fp);

    return ret;
}/*}}}*/

Code DataProcess::CheckNumOfColumns(const std::string &src_file)
{/*{{{*/
    if (src_file.empty()) return kInvalidParam;

    LOG_ERR("[Begin] to check file:%s! delim:%c, num columns:%d, number of hash files:%d", src_file.c_str(), delim_, num_of_columns_, num_of_hash_files_);

    Code ret = kOk;
    FILE *fp = fopen(src_file.c_str(), "r");
    if (fp == NULL)
    {
        LOG_ERR("Failed to open file:%s, errno:%d, err:%s", src_file.c_str(), errno, strerror(errno));
        return kOpenDirFailed;
    }

    int total_line = 0;
    int valid_line = 0;
    int invalid_line = 0;
    while (true)
    {/*{{{*/
        ++total_line;
        std::string content;
        ret = ReadData(&content, fp);
        if (ret == kFileIsEnd)
        {
            --total_line;
            ret = kOk;
            break;
        }
        else if (ret != kOk)
        {
            LOG_ERR("Failed to read line:%d, ret:%d", total_line, ret);
            ++invalid_line;
            continue;
        }

        int elements_of_line = 0;
        ret = GetNumOfElements(content, delim_, &elements_of_line);
        if (ret != kOk)
        {
            LOG_ERR("Failed to get elements of line:%d, content:%s! ret:%d",
                    total_line, content.c_str(), ret);
            ++invalid_line;
            continue;
        }

        if (elements_of_line != num_of_columns_)
        {
            LOG_ERR("Invalid columns of line:%d, content:%s", total_line, content.c_str());
            ++invalid_line;
            continue;
        }

        ++valid_line;
    }/*}}}*/

    fclose(fp);
    fp = NULL;

    if (ret == kOk && invalid_line == 0)
    {
        LOG_ERR("[Success] to check file:%s, total_line:%d", src_file.c_str(), total_line);
    }
    else
    {
        LOG_ERR("[Failed] to check file:%s, total_line:%d, valid_line:%d, invalid_line:%d",
                src_file.c_str(), total_line, valid_line, invalid_line);
        ret = kCheckFileFailed;
    }

    LOG_ERR("[End] to check file:%s! delim:%d, num columns:%d\n", src_file.c_str(), delim_, num_of_columns_);

    return ret;
}/*}}}*/

Code DataProcess::CheckNumOfColumns(const std::vector<std::string> &src_files)
{/*{{{*/
    Code ret = kOk;
    int failed_file_num = 0;
    std::vector<std::string>::const_iterator it;
    for (it = src_files.begin(); it != src_files.end(); ++it)
    {
        Code r = CheckNumOfColumns(*it);
        if (r != kOk)
        {
            ++failed_file_num;
            ret = r;
        }
    }

    LOG_ERR("[Sum] check total files:%u, total successful files:%u, part failed files:%d\n",
            src_files.size(), src_files.size()-failed_file_num, failed_file_num);

    return ret;
}/*}}}*/

Code DataProcess::CheckNumOfColumnsOfDir(const std::string &src_dir)
{/*{{{*/
    std::vector<std::string> src_files;
    Code ret = GetNormalFilesPath(src_dir, &src_files);
    if (ret != kOk)
    {
        LOG_ERR("Failed to get files of dir:%s, ret:%d", src_dir.c_str(), ret);
        return ret;
    }

    return CheckNumOfColumns(src_files);
}/*}}}*/

Code DataProcess::HashFiles(const std::vector<std::string> &src_files, const std::vector<std::string> &dst_dirs)
{/*{{{*/
    if (src_files.empty() || dst_dirs.empty()) return kInvalidParam;
    Code ret = kOk;

    assert(num_of_hash_files_ != 0);
    std::vector<std::string>::const_iterator src_it;
    // deal with over flow problem
    int num_of_single_dir = (num_of_hash_files_ + dst_dirs.size() - 1) / dst_dirs.size();

    std::map<std::string, std::pair<FILE*, std::vector<std::string> > > hash_files;
    std::map<std::string, std::pair<FILE *, std::vector<std::string> > >::iterator hash_files_it;
    for (int i = 0; i < num_of_hash_files_; ++i)
    {/*{{{*/
        char buf[16] = {0};
        snprintf(buf, sizeof(buf), "%05d", i);
        std::string hash_file_path = dst_dirs[i/num_of_single_dir] + "/" + buf;
        FILE *tmp_fp = fopen(hash_file_path.c_str(), "a+");
        if (tmp_fp == NULL)
        {
            LOG_ERR("Failed to open hash file path:%s, errno:%d, err:%s", hash_file_path.c_str(), 
                    errno, strerror(errno));

            ret = kOpenFileFailed;
            goto finish;
        }
        hash_files.insert(std::pair<std::string, std::pair<FILE*, std::vector<std::string> > >(hash_file_path, std::pair<FILE*, std::vector<std::string> >(tmp_fp, std::vector<std::string>())));
    }/*}}}*/

    for (src_it = src_files.begin(); src_it != src_files.end(); ++src_it)
    {/*{{{*/
        FILE *fp = fopen(src_it->c_str(), "r");
        if (fp == NULL)
        {
            LOG_ERR("Failed to open src_file:%s", src_it->c_str());

            ret = kOpenFileFailed;
            goto finish;
        }

        LOG_ERR("[Begin] to hash file:%s", src_it->c_str());
        int num_of_line = 0;
        int num_of_succ_line = 0;
        int invalid_line = 0;
        while (true)
        {/*{{{*/
            ++num_of_line;
            std::string content;
            ret = ReadData(&content, fp);
            if (ret == kFileIsEnd)
            {/*{{{*/
                --num_of_line;

                fclose(fp);
                fp = NULL;

                std::vector<std::string>::iterator cnt_it;
                for (hash_files_it = hash_files.begin(); hash_files_it != hash_files.end(); ++hash_files_it)
                {
                    for (cnt_it = hash_files_it->second.second.begin(); cnt_it != hash_files_it->second.second.end(); ++cnt_it)
                    {
                        WriteData(*cnt_it, hash_files_it->second.first);
                        ++num_of_succ_line;
                    }
                    hash_files_it->second.second.clear();
                }
                LOG_ERR("[Finish] hash file:%s, total line:%d, success line:%d, invalid line:%d",
                            src_it->c_str(), num_of_line, num_of_succ_line, invalid_line);
                LOG_ERR("[End] to hash file:%s\n", src_it->c_str());

                ret = kOk;
                break;
            }/*}}}*/
            else if (ret != kOk)
            {
                LOG_ERR("Failed to read line:%d, ret:%d!", num_of_line, ret);
                fclose(fp);
                fp = NULL;
                break;
            }

            // Check num of columns if need
            if (check_num_of_columns_flag_)
            {/*{{{*/
                int elements_of_line = 0;
                ret = GetNumOfElements(content, delim_, &elements_of_line);
                if (ret != kOk)
                {
                    LOG_ERR("Failed to get elements of line:%d, content:%s! ret:%d",
                            num_of_line, content.c_str(), ret);
                    ++invalid_line;
                    continue;
                }

                if (elements_of_line != num_of_columns_)
                {
                    LOG_ERR("Invalid columns of line:%d, content:%s", num_of_line, content.c_str());
                    ++invalid_line;
                    continue;
                }
            }/*}}}*/

            std::string key;
            ret = GetElementOfIndex(content, first_key_index_, delim_, &key);
            if (ret != kOk)
            {
                LOG_ERR("Failed to get index:%d element of line:%d, content:%s! ret:%d",
                        first_key_index_, num_of_line, content.c_str(), ret);
                ++invalid_line;
                ret = kOk;
                continue;
            }

            int file_num = 0;
            uint64_t num_key = 0;
            switch (way_of_hash_)
            {
                case kBKDRHash:
                    file_num = BKDRHash(key.c_str()) % num_of_hash_files_;
                    break;
                case kMold:
                    ret = GetUInt64(key, &num_key);
                    if (ret != kOk)
                    {
                        LOG_ERR("Failed to number of key:%s", key.c_str());
                        fclose(fp);
                        fp = NULL;
                        goto finish;
                    }
                    file_num = num_key % num_of_hash_files_;
                    break;
                default:
                    LOG_ERR("Invalid way of hash:%d", way_of_hash_);
                    fclose(fp);
                    fp = NULL;
                    goto finish;
            }
            char buf[16] = {0};
            snprintf(buf, sizeof(buf), "%05d", file_num);
            std::string hash_file_path = dst_dirs[file_num/num_of_single_dir] + "/" + buf;

            hash_files_it = hash_files.find(hash_file_path);
            if (hash_files_it == hash_files.end())
            {
                LOG_ERR("Failed to find hash path:%s", hash_file_path.c_str());
                ++invalid_line;
                ret = kOk;
                continue;
            }

            hash_files_it->second.second.push_back(content);
        }/*}}}*/
    }/*}}}*/
    
finish:
    for (hash_files_it = hash_files.begin(); hash_files_it != hash_files.end(); ++hash_files_it)
    {
        fclose(hash_files_it->second.first);
    }

    return ret;
}/*}}}*/

Code DataProcess::HashFile(const std::string &src_file, const std::vector<std::string> &dst_dirs)
{/*{{{*/
    if (src_file.empty() || dst_dirs.empty()) return kInvalidParam;

    std::vector<std::string> src_files;
    src_files.push_back(src_file);

    return HashFiles(src_files, dst_dirs);
}/*}}}*/

Code DataProcess::HashFile(const std::string &src_file, const std::string &dst_dir)
{/*{{{*/
    if (src_file.empty() || dst_dir.empty()) return kInvalidParam;

    std::vector<std::string> src_files;
    src_files.push_back(src_file);

    std::vector<std::string> dst_dirs;
    dst_dirs.push_back(dst_dir);

    return HashFiles(src_files, dst_dirs);
}/*}}}*/

Code DataProcess::HashFiles(const std::string &src_dir, const std::vector<std::string> &dst_dirs)
{/*{{{*/
    if (src_dir.empty() || dst_dirs.empty()) return kInvalidParam;

    std::vector<std::string> src_files;
    Code ret = GetNormalFilesPath(src_dir, &src_files);
    if (ret != kOk)
    {
        LOG_ERR("Failed to get normal files of dir:%s, ret:%d", src_dir.c_str(), ret);
        return ret;
    }

    return HashFiles(src_files, dst_dirs);
}/*}}}*/

Code DataProcess::HashFiles(const std::string &src_dir, const std::string &dst_dir)
{/*{{{*/
    if (src_dir.empty() || dst_dir.empty()) return kInvalidParam;

    std::vector<std::string> src_files;
    Code ret = GetNormalFilesPath(src_dir, &src_files);
    if (ret != kOk)
    {
        LOG_ERR("Failed to get normal files of dir:%s, ret:%d", src_dir.c_str(), ret);
        return ret;
    }

    std::vector<std::string> dst_dirs;
    dst_dirs.push_back(dst_dir);

    return HashFiles(src_files, dst_dirs);
}/*}}}*/

Code DataProcess::SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir)
{/*{{{*/
    Code ret = kOk;

    switch (num_of_sorting_key_)
    {
        case 1: // sort only one key
            if (first_key_sorting_ == kAscend)
            {
                return SortFile(src_file, dst_file, back_dir, std::less<std::string>());
            }
            else if (first_key_sorting_ == kDescend)
            {
                return SortFile(src_file, dst_file, back_dir, std::greater<std::string>());
            }
            else
            {
                LOG_ERR("Invalid sort way:%d", first_key_sorting_);
                ret = kInvalidParam;
            }
            break;

        case 2: // sort first key and second key
            if (first_key_sorting_ == kAscend && second_key_sorting_ == kAscend)
            {
                return SortFile(src_file, dst_file, back_dir,
                                std::less<std::string>(), std::less<std::string>());
            }
            else if (first_key_sorting_ == kAscend && second_key_sorting_ == kDescend)
            {
                return SortFile(src_file, dst_file, back_dir, 
                                std::less<std::string>(), std::greater<std::string>());
            }
            else if (first_key_sorting_ == kDescend && second_key_sorting_ == kAscend)
            {
                return SortFile(src_file, dst_file, back_dir,
                                std::greater<std::string>(), std::less<std::string>());
            }
            else if (first_key_sorting_ == kDescend && second_key_sorting_ == kDescend)
            {
                return SortFile(src_file, dst_file, back_dir,
                                std::greater<std::string>(), std::greater<std::string>());
            }
            else
            {
                LOG_ERR("Invalid first key sort way:%d and second key sort way:%d", 
                        first_key_sorting_, second_key_sorting_);
            }
            break;
            
        default:
            LOG_ERR("Invalid num of sorting key:%d", num_of_sorting_key_);
            ret = kInvalidParam;
            break;
    }

    return ret;
}/*}}}*/

template <typename FirstComp>
Code DataProcess::SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir, const FirstComp &first_comp)
{/*{{{*/
    if (src_file.empty() || dst_file.empty() || back_dir.empty()) return kInvalidParam;

    LOG_INFO("[Begin] to sort src_file:%s to dst_file:%s, backup_dir:%s, first_key_index:%d",
            src_file.c_str(), dst_file.c_str(), back_dir.c_str(), first_key_index_);

    Code ret = kOk;
    FILE *src_fp = fopen(src_file.c_str(), "r");
    if (src_fp == NULL)
    {
        LOG_ERR("Failed to open src_file:%s", src_file.c_str());
        return kOpenFileFailed;
    }

    FILE *dst_fp = fopen(dst_file.c_str(), "w+");
    if (dst_fp == NULL)
    {
        // TODO: close src_fp
        LOG_ERR("Failed to open dst_file:%s", dst_file.c_str());
        return kOpenFileFailed;
    }

    std::string invalid_cnt_dir = back_dir + "/" + kInvalidCntSuffix;
    ret = CreateDir(invalid_cnt_dir);
    if (ret != kOk)
    {
        // TODO: close src_fp and dst_fp
        LOG_ERR("Failed to create dir:%s, ret:%d", invalid_cnt_dir.c_str(), ret);
        return ret;
    }

    std::string tmp_path = src_file;
    std::string base_name = basename(const_cast<char*>(tmp_path.c_str()));
    std::string invalid_cnt_path = invalid_cnt_dir + "/" + base_name + "." + kInvalidCntSuffix;
    FILE *invalid_cnt_fp = fopen(invalid_cnt_path.c_str(), "a+");
    if (invalid_cnt_fp == NULL)
    {
        // TODO: close src_fp and dst_fp
        LOG_ERR("Failed to open invalid_cnt_path:%s", invalid_cnt_path.c_str());
        return kOpenFileFailed;
    }

    // Sort stat file: the stat information of a record
    // Format: key:file_position:count
    std::string stat_dir = back_dir + "/" + kSortStatSuffix;
    ret = CreateDir(stat_dir);
    if (ret != kOk)
    {
        // TODO: close src_fp and dst_fp and invalid_cnt_fp
        LOG_ERR("Failed to create dir:%s, ret:%d", stat_dir.c_str(), ret);
        return ret;
    }

    std::string stat_path = stat_dir + "/" + base_name + "." + kSortStatSuffix;
    FILE *stat_fp = fopen(stat_path.c_str(), "a+");
    if (stat_fp == NULL)
    {
        // TODO: close src_fp and dst_fp and invalid_cnt_fp
        LOG_ERR("Failed to open stat_path:%s", stat_path.c_str());
        return kOpenFileFailed;
    }

    int num_of_line = 0;
    int num_of_succ_line = 0;
    int num_of_invalid_cnt = 0;
    int64_t file_pos = 0;
    std::multimap<std::string, std::string, FirstComp> containers;
    typename std::multimap<std::string, std::string, FirstComp>::iterator containers_it;
    while (true)
    {
        ++num_of_line;
        std::string content;
        ret = ReadData(&content, src_fp);
        if (ret == kFileIsEnd)
        {/*{{{*/
            --num_of_line;

            for (containers_it = containers.begin(); containers_it != containers.end(); ++containers_it)
            {
                file_pos = ftell(dst_fp);
                WriteData(containers_it->second, dst_fp);
                ++num_of_succ_line;
                WriteStat(containers_it->first, file_pos, 1, kStatDelim, stat_fp);
            }

            LOG_ERR("[Finish] sort src file:%s to dst_file:%s,total line:%d,success line:%d,invalid line:%d",
                    src_file.c_str(), dst_file.c_str(), num_of_line, 
                    num_of_succ_line, num_of_invalid_cnt);
            LOG_ERR("[End] to sort src file:%s to dst_file:%s\n", src_file.c_str(), dst_file.c_str());

            ret = kOk;
            break;
        }/*}}}*/
        else if (ret != kOk)
        {
            LOG_ERR("Failed to read line:%d, ret:%d", num_of_line, ret);
            break;
        }

        // Check num of columns if need
        if (check_num_of_columns_flag_)
        {/*{{{*/
            int elements_of_line = 0;
            ret = GetNumOfElements(content, delim_, &elements_of_line);
            if (ret != kOk)
            {
                LOG_ERR("Failed to get elements of line:%d, content:%s! ret:%d",
                        num_of_line, content.c_str(), ret);
                WriteData(content, invalid_cnt_fp);
                ++num_of_invalid_cnt;
                ret = kOk;
                continue;
            }

            if (elements_of_line != num_of_columns_)
            {
                LOG_ERR("Invalid columns of line:%d, content:%s", num_of_line, content.c_str());
                WriteData(content, invalid_cnt_fp);
                ++num_of_invalid_cnt;
                continue;
            }
        }/*}}}*/

        std::string key;
        ret = GetElementOfIndex(content, first_key_index_, delim_, &key);
        if (ret != kOk)
        {
            LOG_ERR("Failed to get index:%d element of line:%d, content:%s! ret:%d",
                    first_key_index_, num_of_line, content.c_str(), ret);
            WriteData(content, invalid_cnt_fp);
            ++num_of_invalid_cnt;
            ret = kOk;
            continue;
        }

        containers.insert(std::pair<std::string, std::string>(key, content));
    }

    fclose(src_fp);
    src_fp = NULL;

    fclose(dst_fp);
    dst_fp = NULL;

    fclose(invalid_cnt_fp);
    invalid_cnt_fp = NULL;

    fclose(stat_fp);
    stat_fp = NULL;

    return ret;
}/*}}}*/

template <typename FirstComp, typename SecondComp>
Code DataProcess::SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir, const FirstComp &first_comp, const SecondComp &second_comp)
{/*{{{*/
    if (src_file.empty() || dst_file.empty() || back_dir.empty()) return kInvalidParam;

    LOG_INFO("[Begin] to sort src_file:%s to dst_file:%s, backup_dir:%s, first_key_index:%d, second_key_index:%d",
            src_file.c_str(), dst_file.c_str(), back_dir.c_str(), first_key_index_, second_key_index_);

    Code ret = kOk;
    FILE *src_fp = fopen(src_file.c_str(), "r");
    if (src_fp == NULL)
    {
        LOG_ERR("Failed to open src_file:%s", src_file.c_str());
        return kOpenFileFailed;
    }

    FILE *dst_fp = fopen(dst_file.c_str(), "w+");
    if (dst_fp == NULL)
    {
        // TODO: close src_fp
        LOG_ERR("Failed to open dst_file:%s", dst_file.c_str());
        return kOpenFileFailed;
    }

    std::string invalid_cnt_dir = back_dir + "/" + kInvalidCntSuffix;
    ret = CreateDir(invalid_cnt_dir);
    if (ret != kOk)
    {
        // TODO: close src_fp and dst_fp
        LOG_ERR("Failed to create dir:%s, ret:%d", invalid_cnt_dir.c_str(), ret);
        return ret;
    }

    std::string tmp_path = src_file;
    std::string base_name = basename(const_cast<char*>(tmp_path.c_str()));
    std::string invalid_cnt_path = invalid_cnt_dir + "/" + base_name + "." + kInvalidCntSuffix;
    FILE *invalid_cnt_fp = fopen(invalid_cnt_path.c_str(), "a+");
    if (invalid_cnt_fp == NULL)
    {
        // TODO: close src_fp and dst_fp
        LOG_ERR("Failed to open invalid_cnt_path:%s", invalid_cnt_path.c_str());
        return kOpenFileFailed;
    }

    // Sort stat file: the stat information of a record
    // Format: key:file_position:count
    std::string stat_dir = back_dir + "/" + kSortStatSuffix;
    ret = CreateDir(stat_dir);
    if (ret != kOk)
    {
        // TODO: close src_fp and dst_fp and invalid_cnt_fp
        LOG_ERR("Failed to create dir:%s, ret:%d", stat_dir.c_str(), ret);
        return ret;
    }

    std::string stat_path = stat_dir + "/" + base_name + "." + kSortStatSuffix;
    FILE *stat_fp = fopen(stat_path.c_str(), "a+");
    if (stat_fp == NULL)
    {
        // TODO: close src_fp and dst_fp and invalid_cnt_fp
        LOG_ERR("Failed to open stat_path:%s", stat_path.c_str());
        return kOpenFileFailed;
    }

    int num_of_line = 0;
    int64_t file_pos = 0;
    int num_of_succ_line = 0;
    int num_of_invalid_cnt = 0;
    std::map<std::string, std::multimap<std::string, std::string, SecondComp>, FirstComp> containers;
    typedef typename std::map<std::string, std::multimap<std::string, std::string, SecondComp>, FirstComp>::iterator containers_iterator;
    containers_iterator containers_it;
    std::pair<containers_iterator, bool> insert_pair;

    while (true)
    {/*{{{*/
        ++num_of_line;
        std::string content;
        ret = ReadData(&content, src_fp);
        if (ret == kFileIsEnd)
        {/*{{{*/
            --num_of_line;

            typename std::multimap<std::string, std::string, SecondComp>::iterator value_it;
            for (containers_it = containers.begin(); containers_it != containers.end(); ++containers_it)
            {
                file_pos = ftell(dst_fp);
                for (value_it = containers_it->second.begin(); value_it != containers_it->second.end(); ++value_it)
                {
                    WriteData(value_it->second, dst_fp);
                    ++num_of_succ_line;
                }
                WriteStat(containers_it->first, file_pos, containers_it->second.size(), kStatDelim, stat_fp);
            }

            LOG_ERR("[Finish] sort src file:%s to dst_file:%s,total line:%d,success line:%d,invalid line:%d",
                    src_file.c_str(), dst_file.c_str(), num_of_line, 
                    num_of_succ_line, num_of_invalid_cnt);
            LOG_ERR("[End] to sort src file:%s to dst_file:%s\n", src_file.c_str(), dst_file.c_str());

            ret = kOk;
            break;
        }/*}}}*/
        else if (ret != kOk)
        {
            LOG_ERR("Failed to read line:%d, ret:%d", num_of_line, ret);
            break;
        }

        // Check num of columns if need
        if (check_num_of_columns_flag_)
        {/*{{{*/
            int elements_of_line = 0;
            ret = GetNumOfElements(content, delim_, &elements_of_line);
            if (ret != kOk)
            {
                LOG_ERR("Failed to get elements of line:%d, content:%s! ret:%d",
                        num_of_line, content.c_str(), ret);
                WriteData(content, invalid_cnt_fp);
                ++num_of_invalid_cnt;
                ret = kOk;
                continue;
            }

            if (elements_of_line != num_of_columns_)
            {
                LOG_ERR("Invalid columns of line:%d, content:%s", num_of_line, content.c_str());
                WriteData(content, invalid_cnt_fp);
                ++num_of_invalid_cnt;
                continue;
            }
        }/*}}}*/

        std::string first_key;
        ret = GetElementOfIndex(content, first_key_index_, delim_, &first_key);
        if (ret != kOk)
        {/*{{{*/
            LOG_ERR("Failed to get index:%d element of line:%d, content:%s! ret:%d",
                    first_key_index_, num_of_line, content.c_str(), ret);
            WriteData(content, invalid_cnt_fp);
            ++num_of_invalid_cnt;
            ret = kOk;
            continue;
        }/*}}}*/

        std::string second_key;
        ret = GetElementOfIndex(content, second_key_index_, delim_, &second_key);
        if (ret != kOk)
        {/*{{{*/
            LOG_ERR("Failed to get index:%d element of line:%d, content:%s! ret:%d",
                    second_key_index_, num_of_line, content.c_str(), ret);
            WriteData(content, invalid_cnt_fp);
            ++num_of_invalid_cnt;
            ret = kOk;
            continue;
        }/*}}}*/

        containers_it = containers.find(first_key);
        if (containers_it == containers.end())
        {
            insert_pair = containers.insert(std::pair<std::string, std::multimap<std::string, std::string, SecondComp> >(first_key, std::multimap<std::string, std::string, SecondComp>()));
            if (!insert_pair.second)
            {
                LOG_ERR("Failed to insert key:%s! line:%d, Map may be full, now exit!", 
                        num_of_line, first_key.c_str());
                break;
            }
            containers_it = insert_pair.first;
        }
        containers_it->second.insert(std::pair<std::string, std::string>(second_key, content));
    }/*}}}*/

    fclose(src_fp);
    src_fp = NULL;

    fclose(dst_fp);
    dst_fp = NULL;

    fclose(invalid_cnt_fp);
    invalid_cnt_fp = NULL;

    return ret;
}/*}}}*/


}

#ifdef _DATA_PROCESS_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;

    SetLogLevel(kInfoLevel);

    char delim = ',';
    int num_of_columns = 4;
    bool check_columns_flag = true;
    int num_of_sorting_key = 1;
    int first_key_index = 1;
    int second_key_index = 0;
    int num_of_hash_files = 10;

    DataProcess data_process(delim, num_of_columns, check_columns_flag, 
                             num_of_sorting_key, first_key_index, second_key_index, num_of_hash_files);
    std::string src_dir = "../data/src";
    Code ret = data_process.CheckNumOfColumnsOfDir(src_dir);
    if (ret != kOk)
    {
        LOG_ERR("Failed to check src_dir:%s, ret:%d\n", src_dir.c_str(), ret);
    }

    // hash files
    std::vector<std::string> dst_hash_dirs;
    dst_hash_dirs.push_back("../data/hash/hash_one");
    dst_hash_dirs.push_back("../data/hash/hash_two");
    dst_hash_dirs.push_back("../data/hash/hash_three");
    ret = data_process.HashFiles(src_dir, dst_hash_dirs);
    if (ret != kOk)
    {
        LOG_ERR("Failed to hash files of src_dir:%s, ret:%d\n", src_dir.c_str(), ret);
    }

    // sort only one key
    std::string src_file_path = "../data/src/aa";
    std::string dst_sort_path = "../data/sort/aa.sort";
    std::string backup_dir = "../data/backup";
    data_process.SetFirstKeySortWay(kDescend);
    ret = data_process.SortFile(src_file_path, dst_sort_path, backup_dir);
    if (ret != kOk)
    {
        LOG_ERR("Failed to sort src file:%s to dst_file:%s, ret:%d\n", src_file_path.c_str(), 
                dst_sort_path.c_str(), backup_dir.c_str());
    }

    // sort two keys;
    src_file_path = "../data/src/aa";
    dst_sort_path = "../data/sort/aa.two_keys.sort";
    backup_dir = "../data/backup";
    data_process.SetFirstKeySortWay(kDescend);
    data_process.SetNumOfSortingKey(2);
    data_process.SetSecondKeyIndex(2);
    data_process.SetSecondKeySortWay(kDescend);
    ret = data_process.SortFile(src_file_path, dst_sort_path, backup_dir);
    if (ret != kOk)
    {
        LOG_ERR("Failed to sort src file:%s to dst_file:%s, ret:%d\n", src_file_path.c_str(),
                dst_sort_path.c_str(), backup_dir.c_str());
    }

    return 0;
}
#endif
