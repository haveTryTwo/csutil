// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "base/ip.h"
#include "base/log.h"
#include "base/time.h"
#include "base/hash.h"
#include "base/util.h"
#include "base/status.h"
#include "base/file_util.h"
#include "data_process/src/data_process.h"

#include "log_check.h"
#include "create_cc_file.h"
#include "file_content_replace.h"

namespace tools
{
void Help(const std::string &program)
{/*{{{*/
    fprintf(stderr,
            "Usage: %s [Option] case_num\n"
            "[-s src_dir/src_file] [-l log_name_prefix] [-p replace_pos] [-r replace_str]\n"
            "case_num:\n"
            "1  [-s src_file] [-l log_name_prefix]: Check log format of src file\n"
            "2  [-s src_dir] [-l log_name_prefix]: Check log format of cplusplus files in dir\n"
            "3  [-s src_file] [-p replace_pos] [-r replace_str]: Repalce content in src file with str\n"
            "4  [-s src_dir] [-p replace_pos] [-r replace_str]: Repalce content in cplusplus files with str\n"
            "5  [-s src_dir] [-d dst_dir] [-m delim] [-c column number] [-h hash number]: Hash files in src directories to dest directory\n"
            "11 [-s src_file] [-l find_name_prefix] [-n log interval lines]: Log 'log_interval_logs' lines if containing find_name_prefix\n"
            "12 [-s dir] [-l find_name_prefix] [-n log interval lines]: Log 'log_interval_logs' lines if containing find_name_prefix in dir\n"
            "21 [-s src_file] [-f func_name]: create a src_file of cplusplus template including function with func_name\n"
            "31 [-s str]: BKDHash this str\n"
            "41 [-t time]: Translate timestamp to Date(YYYY-mm-dd HH:MM:SS)\n"
            "42 [-d date]: Translate Date(YYYY-mm-dd HH:MM:SS) to timestamp\n"
            "51 [-s string ip]: Translate string ip(xxx.xxx.xxx.xxx) to int ip\n"
            "52 [-i int ip]: Translate int ip to string ip(xxx.xxx.xxx.xxx)\n"
            ,program.c_str());
}/*}}}*/
}

#ifdef _TOOLS_MAIN_TEST_
int main(int argc, char *argv[])
{/*{{{*/
    using namespace tools;
    if (argc < 2)
    {
        Help(argv[0]);
        return -1;
    }

    base::SetLogLevel(base::kInfoLevel);
//    base::Code r = base::GetAndSetMaxFileNo();
//    if (r != base::kOk)
//    {
//        fprintf(stderr, "Failed to get and set max file no! ret:%d\n", r);
//        return -1;
//    }

    int32_t opt = 0;
    std::string src_path;
    std::string log_name;
    uint64_t replace_pos = 0;
    std::string replace_str;
    std::string src_dir;
    std::string dst_dir;
    std::string str;
    std::string func_name;
    std::string date;
    char delim = 0;
    int hash_numbers = 0;
    int column_numbers = 0;
    int log_interval_lines = 0;
    long time = 0;
    int int_value = 0;
    while ((opt = getopt(argc, argv, "s:l:p:r:d:c:h:d:m:n:f:t:i:")) != -1)
    {/*{{{*/
        switch (opt)
        {
            case 's':
                src_path = optarg;
                src_dir = optarg;
                str = optarg;
                break;
            case 'l':
                log_name = optarg;
                break;
            case 'i':
                int_value = atoi(optarg);
                break;
            case 'p':
                replace_pos = atoi(optarg);
                break;
            case 'r':
                replace_str = optarg;
                break;
            case 'd':
                dst_dir = optarg;
                date = optarg;
                break;
            case 'c':
                column_numbers = atoi(optarg);
                break;
            case 'm':
                delim = *optarg;
                break;
            case 'h':
                hash_numbers = atoi(optarg);
                break;
            case 'n':
                log_interval_lines = atoi(optarg);
                break;
            case 'f':
                func_name = optarg;
                break;
            case 't':
                time = atol(optarg);
                break;
            default:
                fprintf(stderr, "Not right options\n");
                Help(argv[0]);
                return -1;
        }
    }/*}}}*/

    int ret = 0;
    if (optind < argc)
    {/*{{{*/
        int num_case = atoi(argv[optind]);
        switch (num_case)
        {
            case 1:
                {/*{{{*/
                    if (src_path.empty() || log_name.empty())
                    {
                        fprintf(stderr, "Invalid src_path or log_name\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = CheckLogFormatForCC(src_path, log_name);
                }/*}}}*/
                break;
            case 2:
                {/*{{{*/
                    if (src_path.empty() || log_name.empty())
                    {
                        fprintf(stderr, "Invalid src_path or log_name\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = CheckLogFormatForCCS(src_path, log_name);
                }/*}}}*/
                break;
            case 3:
                {/*{{{*/
                    if (src_path.empty() || replace_str.empty())
                    {
                        fprintf(stderr, "Invalid src_path or replace_str\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = base::ReplaceFileContent(src_path, replace_pos, replace_str.size(), replace_str);
                }/*}}}*/
                break;
            case 4:
                {/*{{{*/
                    if (src_path.empty() || replace_str.empty())
                    {
                        fprintf(stderr, "Invalid src_path or replace_str\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = ReplaceAllCCFileContent(src_path, replace_pos, replace_str.size(), replace_str);
                }/*}}}*/
                break;
            case 5:
                {/*{{{*/
                    if (src_dir.empty() || dst_dir.empty() || hash_numbers == 0)
                    {
                        fprintf(stderr, "Invalid src_dir or dst_dir or hash_numbers\n");
                        Help(argv[0]);
                        return -1;
                    }
                    bool  check_columns_flag = true;
                    int num_of_sorting_key = 1;
                    int first_key_index = 1;
                    int second_key_index = 0;
                    base::DataProcess *data_process = new base::DataProcess(delim, column_numbers, check_columns_flag, num_of_sorting_key, first_key_index, second_key_index, hash_numbers);
                    ret = data_process->SetHashWay(base::kMold);
                    if (ret != base::kOk)
                    {
                        fprintf(stderr, "Failed to set hash way, ret:%d\n", ret);
                        delete data_process;
                        data_process = NULL;
                        return -1;
                    }
                    ret = data_process->CheckNumOfColumnsOfDir(src_dir);
                    if (ret != base::kOk)
                    {
                        fprintf(stderr, "Failed to check src_dir:%s, ret:%d\n", src_dir.c_str(), ret);
                        delete data_process;
                        data_process = NULL;
                        return -1;
                    }

                    ret = data_process->HashFiles(src_dir, dst_dir);
                    if (ret != base::kOk)
                    {
                        fprintf(stderr, "Failed to hash files of src_dir:%s, ret:%d\n", src_dir.c_str(), ret);
                        delete data_process;
                        data_process = NULL;
                        return -1;
                    }

                    delete data_process;
                    data_process = NULL;
                }/*}}}*/
                break;
            case 11:
                {/*{{{*/
                    if (src_path.empty() || log_name.empty() || log_interval_lines < 0)
                    {
                        fprintf(stderr, "Invalid src_path or replace_str\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = LogContent(src_path, log_name, log_interval_lines);
                }/*}}}*/
                break;
            case 12:
                {/*{{{*/
                    if (src_path.empty() || log_name.empty() || log_interval_lines < 0)
                    {
                        fprintf(stderr, "Invalid src_path or replace_str\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = LogContentInDir(src_path, log_name, log_interval_lines);
                }/*}}}*/
            case 21:
                {/*{{{*/
                    if (src_path.empty() || func_name.empty())
                    {
                        fprintf(stderr, "Invalid src_path or function name\n");
                        Help(argv[0]);
                        return -1;
                    }
                    ret = CreateCCFile(src_path, func_name);
                }/*}}}*/
                break;
            case 31:
                {/*{{{*/
                    uint32_t hash_value = base::BKDRHash(str.c_str());
                    fprintf(stderr, "BKDRHash of %s is %u\n", str.c_str(), hash_value);
                }/*}}}*/
                break;
            case 41:
                {/*{{{*/
                    std::string date;
                    ret = base::Time::GetDate(time, &date);
                    fprintf(stderr, "%s\n", date.c_str());
                }/*}}}*/
                break;
            case 42:
                {/*{{{*/
                    time_t time;
                    ret = base::Time::GetSecond(date, &time);
                    fprintf(stderr, "%zu\n", time);
                }/*}}}*/
                break;
            case 51:
                {/*{{{*/
                    uint32_t uint_ip = 0;
                    ret = base::GetUIntIpByStr(str, &uint_ip);
                    fprintf(stderr, "%u\n", uint_ip);
                }/*}}}*/
                break;
            case 52:
                {/*{{{*/
                    std::string str_ip;
                    ret = base::GetStrIpByUint(int_value, &str_ip);
                    fprintf(stderr, "%s\n", str_ip.c_str());
                }/*}}}*/
                break;
            default:
                fprintf(stderr, "Invalid case num:%d\n", num_case);
                Help(argv[0]);
                return -1;
        }
    }/*}}}*/
    else
    {
        Help(argv[0]);
        return -1;
    }

	return ret; 
}/*}}}*/
#endif
