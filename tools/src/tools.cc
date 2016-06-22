// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "base/log.h"
#include "base/util.h"
#include "base/status.h"

#include "log_check.h"

namespace tools
{
void Help(const std::string &program)
{/*{{{*/
    fprintf(stderr,
            "Usage: %s [Option] case_num\n"
            "[-s src_dir/src_file] [-l log_name_prefix]\n"
            "case_num:\n"
            "1  [-s src_file] [-l log_name_prefix]: Check log format of src file\n"
            "2  [-s src_dir] [-l log_name_prefix]: Check log format of cplusplus files in dir\n"
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
    while ((opt = getopt(argc, argv, "s:l:")) != -1)
    {/*{{{*/
        switch (opt)
        {
            case 's':
                src_path = optarg;
                break;
            case 'l':
                log_name = optarg;
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
                    ret = CheckLogFormat(src_path, log_name);
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
                    ret = CheckLogFormatForCC(src_path, log_name);
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
