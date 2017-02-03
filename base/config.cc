// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "base/util.h"
#include "base/common.h"
#include "base/config.h"

namespace base
{

Config::Config()
{
}

Config::~Config()
{
}

Code Config::LoadFile(const std::string &path)
{/*{{{*/
    if (path.empty()) return kInvalidParam;

    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL) return kOpenError;

    Code ret = kOk;
    char buf[kBufLen];
    while (!feof(fp) && !ferror(fp))
    {
        char *tmp = fgets(buf, sizeof(buf), fp);
        if (tmp == NULL)
        {
            if (feof(fp)) break;

            ret = kReadError;
            break;
        }

        SetConf(buf);
    }
    fclose(fp);
    fp = NULL;

    return ret;
}/*}}}*/

Code Config::GetValue(const std::string &key, std::string *value)
{/*{{{*/
    std::map<std::string, std::string>::const_iterator it = confs_.find(key);
    if (it == confs_.end())
        return kNotFound;

    if (value != NULL)
        value->assign(it->second);

    return kOk;
}/*}}}*/

Code Config::GetInt32Value(const std::string &key, int *value)
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    int64_t int64_value = 0;
    Code ret = GetInt64Value(key, &int64_value);
    if (ret != kOk) return ret;

    *value = (int)int64_value;

    return kOk;
}/*}}}*/

Code Config::GetInt64Value(const std::string &key, int64_t *value)
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    std::string str_value;
    Code ret = GetValue(key, &str_value);
    if (ret != kOk) return ret;

    errno = 0;
    char *end_ptr = NULL;
    long long v = strtoll(str_value.c_str(), &end_ptr, 0);
    if ((errno == ERANGE && (v == LONG_MAX || v == LONG_MIN)) ||
            (errno != 0 && v == 0))
        return kStrtollFailed;
    if (end_ptr == key.c_str()) return kNoDigits;
    if (*end_ptr != '\0') return kNotAllDigits;

    *value = v;
    return kOk;
}/*}}}*/

Code Config::GetValue(const std::string &key, std::string *value) const
{/*{{{*/
    std::map<std::string, std::string>::const_iterator it = confs_.find(key);
    if (it == confs_.end())
        return kNotFound;

    if (value != NULL)
        value->assign(it->second);

    return kOk;
}/*}}}*/

Code Config::GetInt32Value(const std::string &key, int *value) const
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    int64_t int64_value = 0;
    Code ret = GetInt64Value(key, &int64_value);
    if (ret != kOk) return ret;

    *value = (int)int64_value;

    return kOk;
}/*}}}*/

Code Config::GetInt64Value(const std::string &key, int64_t *value) const
{/*{{{*/
    if (value == NULL) return kInvalidParam;

    std::string str_value;
    Code ret = GetValue(key, &str_value);
    if (ret != kOk) return ret;

    errno = 0;
    char *end_ptr = NULL;
    long long v = strtoll(str_value.c_str(), &end_ptr, 0);
    if ((errno == ERANGE && (v == LONG_MAX || v == LONG_MIN)) ||
            (errno != 0 && v == 0))
        return kStrtollFailed;
    if (end_ptr == str_value.c_str()) return kNoDigits;
    if (*end_ptr != '\0') return kNotAllDigits;

    *value = v;
    return kOk;
}/*}}}*/

void Config::Print()
{/*{{{*/
    std::map<std::string, std::string>::const_iterator it = confs_.begin();
    while (it != confs_.end())
    {
        fprintf(stderr, "%s : %s\n", it->first.c_str(), it->second.c_str());
        ++it;
    }
}/*}}}*/

Code Config::SetConf(const std::string &cnt)
{/*{{{*/
    // trim the \n
    std::string tmp_cnt;
    Code ret = TrimRight(cnt, kNewLine, &tmp_cnt);
    assert(ret == kOk);

    // trim the white space
    std::string tmp_out_cnt;
    ret = Trim(tmp_cnt, kDefaultDelim, &tmp_out_cnt);
    assert(ret == kOk);

    // check whether is comment
    if (tmp_out_cnt.empty() || tmp_out_cnt.data()[0] == kDefaultCommitChar)
        return kOk;

    // separete the key and value
    std::string key;
    std::string value;
    ret = Separate(tmp_out_cnt, kEqualStr, &key, &value); 
    if (ret != kOk) return ret;

    std::string key_out;
    ret = TrimRight(key, kDefaultDelim, &key_out);
    assert(ret == kOk);

    std::string value_out;
    ret = TrimLeft(value, kDefaultDelim, &value_out);

    if (!key.empty() && !value.empty())
        confs_.insert(std::make_pair<std::string, std::string>(key_out, value_out));

    return kOk;
}/*}}}*/

}

#ifdef _CONFIG_MAIN_TEST_
int main(int argc, char *argv[])
{
    using namespace base;
    Config conf;
    std::string path = "../demo/conf/cs.conf";
    conf.LoadFile(path);
    conf.Print();

    std::string key("daemon");
    int daemon_flags = 0;
    Code ret = conf.GetInt32Value(key, &daemon_flags);
    if (ret != kOk)
    {
        fprintf(stderr, "Get daemon flags failed! ret:%d\n", ret);
        return -1;
    }
    fprintf(stderr, "key:%s, value:%d\n", key.c_str(), daemon_flags);

    return 0;
}
#endif
