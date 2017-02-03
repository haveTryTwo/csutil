// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <assert.h>

#include "base/util.h"
#include "base/file_util.h"

#include "log_check.h"

namespace tools
{

base::Code CheckLogFormat(const std::string &cnt, bool *is_satisfied)
{/*{{{*/
    if (cnt.empty() || is_satisfied == NULL) return base::kInvalidParam;

    *is_satisfied = false;

    base::Code ret = base::kOk;

    int num_of_percent = 0;
    int num_of_comma = 0;
    int num_of_parentheses_in_using = 0;
    bool has_parentheses_flag = false;
    bool is_double_quotes_found = false;
    bool is_double_quotes_finish = false;

    char percent = '%';
    char comma = ',';
    char back_slash = '\\';
    char double_quotes = '"';
    char left_parentheses = '(';
    char right_parentheses = ')';

    for (int i = 0; i < (int)cnt.size(); ++i)
    {/*{{{*/
        if (!is_double_quotes_found)
        {
            if (cnt.data()[i] != double_quotes)
                continue;

            is_double_quotes_found = true;
            continue;
        }

        if (!is_double_quotes_finish)
        {
            if (cnt.data()[i] != double_quotes)
            {
                if (cnt.data()[i] == percent)
                    ++num_of_percent;

                continue;
            }
            else
            {
                if (i <= 0)
                {
                    fprintf(stderr, "Invalid palce that should not be arrived!\n");
                    ret = base::kInvalidPlace;
                    break;
                }

                // Here may be \"
                if (cnt.data()[i-1] == back_slash)
                    continue;

                is_double_quotes_finish = true;
                continue;
            }
        }

        // new double quotes line
        if (num_of_comma == 0 && !has_parentheses_flag && cnt.data()[i] == double_quotes)
        {
            is_double_quotes_finish = false;
            continue;
        }

        if (cnt.data()[i] == left_parentheses)
        {
            ++num_of_parentheses_in_using;
            has_parentheses_flag = true;
            continue;
        }

        if (cnt.data()[i] == right_parentheses)
        {
            --num_of_parentheses_in_using;
            continue;
        }

        if (num_of_parentheses_in_using == 0 && cnt.data()[i] == comma)
        {
            ++num_of_comma;
            continue;
        }
    }/*}}}*/

    if (num_of_percent == num_of_comma)
    {
        *is_satisfied = true;
    }
    else
    {
        fprintf(stderr, "num_of_percent:%d, num_of_comma:%d\n", num_of_percent, num_of_comma);
    }

    return ret;
}/*}}}*/

base::Code CheckLogFormat(const std::string &path, const std::string &log_name, std::map<uint32_t, bool> *result)
{/*{{{*/
    if (path.empty() || log_name.empty() || result == NULL) return base::kInvalidParam;

    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open path:%s\n", path.c_str());
        return base::kOpenFileFailed;
    }

    fprintf(stderr, "[BEGIN] Start to read file:%s\n", path.c_str());

    base::Code ret = base::kOk;
    uint32_t line = 0;
    std::string cnt;
    uint32_t new_log_line = 0;
    bool is_new_log_line = false;
    while (true)
    {/*{{{*/
        ++line;
        std::string tmp_cnt;
        ret = base::PumpStringData(&tmp_cnt, fp);
        if (ret != base::kOk)
        {/*{{{*/
            if (ret == base::kFileIsEnd)
            {
                --line;
                fprintf(stderr, "[END] File:%s has been finished!\n", path.c_str());
                ret = base::kOk;
                break;
            }
            else
            {
                fprintf(stderr, "[FAILED] Reading file:%s failed!\n", path.c_str());
                break;
            }
        }/*}}}*/

        if (!is_new_log_line)
        {/*{{{*/
            std::string left_delims = " \t";
            std::string out_str;
            ret = base::TrimLeft(tmp_cnt, left_delims, &out_str); 
            assert(ret == base::kOk);

            if (out_str.compare(0, log_name.size(), log_name) != 0)
                continue;

            is_new_log_line = true;
            new_log_line = line;
        }/*}}}*/

        cnt.append(tmp_cnt);

        std::string right_delims = " \t\r\n";
        std::string out_str;
        ret = base::TrimRight(tmp_cnt, right_delims, &out_str);
        assert(ret == base::kOk);
        if (out_str.size() == 0 || (out_str.size() > 0 && out_str.data()[out_str.size()-1] != ';'))
            continue;

        // check log content
        bool is_satisfied = false;
        ret = CheckLogFormat(cnt, &is_satisfied);
        assert(ret == base::kOk);
        if (is_satisfied)
        {
            result->insert(std::make_pair<uint32_t, bool>(new_log_line, true));
        }
        else
        {
            fprintf(stderr, "[NOT RIGHT] check log format failed, Line:%u, cnt:\n%s\n",
                    new_log_line, cnt.c_str());
                    result->insert(std::make_pair<uint32_t, bool>(new_log_line, false));
        }

        is_new_log_line = false;
        new_log_line = 0;
        cnt.clear();
    }/*}}}*/

    fclose(fp);
    fp = NULL;

    return ret;
}/*}}}*/

base::Code CheckLogFormat(const std::string &path, const std::string &log_name)
{/*{{{*/
    if (path.empty() || log_name.empty()) return base::kInvalidParam;

    std::map<uint32_t, bool> result;
    base::Code ret = CheckLogFormat(path, log_name, &result);
    assert(ret == base::kOk);

    std::map<uint32_t, bool>::iterator map_it;
    uint32_t num_of_satisfied = 0;
    uint32_t num_of_not_satisfied = 0;
    for (map_it = result.begin(); map_it != result.end(); ++map_it)
    {
        if (map_it->second)
            ++num_of_satisfied;
        else
            ++num_of_not_satisfied;
    }

    assert(result.size() == num_of_satisfied+num_of_not_satisfied);
    fprintf(stderr, "File:%s, total log line:%zu, right line:%u, not right line:%u\n", 
            path.c_str(), result.size(), num_of_satisfied, num_of_not_satisfied);

    return ret;
}/*}}}*/

base::Code CheckLogFormatForCC(const std::string &dir, const std::string &log_name)
{/*{{{*/
    if (dir.empty() || log_name.empty()) return base::kInvalidParam;

    std::vector<std::string> files_path;
    base::Code ret = base::GetNormalFilesPath(dir, &files_path);
    assert(ret == base::kOk);

    uint32_t total_num_of_satisfied = 0;
    uint32_t total_num_of_not_satisfied = 0;
    std::vector<std::string>::iterator vec_it;
    for (vec_it = files_path.begin(); vec_it != files_path.end(); ++vec_it)
    {
        bool is_cpp_flags = false;
        ret = base::CheckIsCplusplusFile(*vec_it, &is_cpp_flags);
        assert(ret == base::kOk);
        if (!is_cpp_flags)
            continue;

        std::map<uint32_t, bool> result;
        ret = CheckLogFormat(*vec_it, log_name, &result);
        assert(ret == base::kOk);

        std::map<uint32_t, bool>::iterator map_it;
        uint32_t num_of_satisfied = 0;
        uint32_t num_of_not_satisfied = 0;
        for (map_it = result.begin(); map_it != result.end(); ++map_it)
        {
            if (map_it->second)
                ++num_of_satisfied;
            else
                ++num_of_not_satisfied;
        }

        assert(result.size() == num_of_satisfied+num_of_not_satisfied);
        fprintf(stderr, "File:%s, total log line:%zu, right line:%u, not right line:%u\n\n", 
                vec_it->c_str(), result.size(), num_of_satisfied, num_of_not_satisfied);

        total_num_of_satisfied += num_of_satisfied;
        total_num_of_not_satisfied += num_of_not_satisfied;
    }

    fprintf(stderr, "\ndir:%s, total log num:%u, right line:%u, not right line:%u\n", 
            dir.c_str(), total_num_of_satisfied+total_num_of_not_satisfied,
            total_num_of_satisfied, total_num_of_not_satisfied);

    return ret;
}/*}}}*/


}

#ifdef _LOG_CHECK_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace tools;

    std::string path = "../../data_process/src/data_process.cc";
    std::string log_name = "LOG";

    CheckLogFormat(path, log_name);
    return 0;
}
#endif
