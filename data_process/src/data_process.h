// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>
#include <string>

#include "base/status.h"

namespace base
{

enum OpFileWay
{
    // reading or writing a line each time!
    // Current function:
    //      read:   PumpStringData(std::string *str, FILE *fp);
    //      write:  DumpStringData(cnost std::string &str, FILE *fp);
    kLine           = 1,

    // reading or writing Len:Content each time!
    // Current function:
    //      read:   PumpBinData(std::string *bin_str, FILE *fp);
    //      write:  DumpBinData(const std::string &bin_str, FILE *fp);
    kLenAndContent  = 2,
};

enum SortWay
{
    // Ascending sorting
    kAscend         = 1,

    // Descendig sorting
    kDescend        = 2,
};

enum HashWay
{
    // Using BKDRHash function
    kBKDRHash        = 1,
};

class DataProcess
{
    public:
        DataProcess(char delim, int num_of_columns, bool check_columns_flag, int num_of_sorting_key, int first_key_index, int second_key_index, int num_of_hash_files);
        ~DataProcess();

    public:
        Code SetDelim(char delim);
        Code SetNumOfColumns(int num_of_columns);
        Code SetCheckCloumnFlag(bool check_columns_flag);
        Code SetReadWay(OpFileWay read_way);
        Code SetWriteWay(OpFileWay write_way);
        Code SetNumOfSortingKey(int num_of_sorting_key);
        Code SetFirstKeyIndex(int first_key_index);
        Code SetIndexFlag(bool set_first_key_index_in_file_flag);
        Code SetFirstKeySortWay(SortWay first_key_sort_way);
        Code SetSecondKeyIndex(int second_key_index);
        Code SetSecondKeySortWay(SortWay second_key_sort_way);
        Code SetNumOfHashFiles(int num_of_hash_files);
        Code SetHashWay(HashWay hash_way);

    public:
        Code ReadData(std::string *content, FILE *fp);
        Code WriteData(const std::string &content, FILE *fp);

        // Note: format of stat: key:file_pos:count
        Code WriteStat(const std::string &key, int64_t file_pos, int count, char stat_delim, FILE *fp);

        Code CheckNumOfColumns(const std::string &src_file);
        Code CheckNumOfColumns(const std::vector<std::string> &src_files);
        Code CheckNumOfColumnsOfDir(const std::string &src_dir);

        Code HashFiles(const std::vector<std::string> &src_files, const std::vector<std::string> &dst_dirs);
        Code HashFile(const std::string &src_file, const std::vector<std::string> &dst_dirs);
        Code HashFile(const std::string &src_file, const std::string &dst_dir);
        Code HashFiles(const std::string &src_dir, const std::vector<std::string> &dst_dirs);
        Code HashFiles(const std::string &src_dir, const std::string &dst_dir);

        Code SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir);
        Code SortFiles(const std::string &src_dir, const std::string &dst_dir, const std::string &back_dir);

        Code MergeFiles(const std::string &src_dir, const std::string &dst_dir, const std::string &file_suffix, const std::string &back_dir);

    private:
        template <typename FirstComp>
        Code SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir, const FirstComp &first_comp);

        template <typename FirstComp, typename SecondComp>
        Code SortFile(const std::string &src_file, const std::string &dst_file, const std::string &back_dir, const FirstComp &first_comp, const SecondComp &second_comp);

    private:
        char delim_;

        int num_of_columns_;

        bool check_num_of_columns_flag_;

        OpFileWay way_of_reading_file_;
        OpFileWay way_of_writing_file_;

        // Current supporting only one key or two keys sorting
        // One key sorting: sort records by the key
        // Two keys sorting: sort records by first key, then sort records with same first key by second key 
        int num_of_sorting_key_; 

        // index position of first key, starting with 1
        int first_key_index_;

        SortWay first_key_sorting_;

        // index position of second key, starting with 1
        // Note: this index may not be used if num_of_sorting_key_ is one
        int second_key_index_;

        SortWay second_key_sorting_;

        // Hash the key of record and store the record in different files
        // The number may be 1000
        int num_of_hash_files_;

        HashWay way_of_hash_;
};

}
