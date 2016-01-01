// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "base/log.h"
#include "base/util.h"
#include "base/common.h"
#include "base/file_util.h"

namespace base
{

Code CreateDir(const std::string &dir_path)
{/*{{{*/
    if (dir_path.empty()) return kInvalidParam;

    int ret = access(dir_path.c_str(), F_OK);
    if (ret == 0) return kOk;
        
    ret = mkdir(dir_path.c_str(), kDefaultDirMode);
    if (ret != 0) return kMkdirFailed;

    return kOk;
}/*}}}*/

Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg)
{/*{{{*/
    int fd = open(path.c_str(), O_RDWR|O_CREAT, kDefaultRegularFileMode);
    if (fd == -1) return kOpenFileFailed;

    char buf[kBufLen] = {0};
    int ret = read(fd, buf, sizeof(buf));
    if (ret == -1) return kReadError;

    if (ret == (int)msg.size() && (memcmp(buf, msg.c_str(), ret) == 0))
    {
        close(fd);
        return kOk;
    }

    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);   // truncate the size of to 0 as the size may be larger than message
    ret = write(fd, msg.c_str(), msg.size());
    if (ret != (int)msg.size())
    {
        close(fd);
        return kWriteError;
    }

    close(fd);
    return kOk;
}/*}}}*/

Code GetNormalFilesName(const std::string &dir_path, std::vector<std::string> *files)
{/*{{{*/
    int ret = 0;
    DIR *dir = NULL;
    struct stat st;
    struct dirent* file = NULL;
    char *file_name = NULL;

    memset(&st, 0, sizeof(struct stat));

    ret = stat(dir_path.c_str(), &st);
    if (ret == -1) return kStatFailed;

    if (!S_ISDIR(st.st_mode)) return kNotDir;

    dir = opendir(dir_path.c_str());
    if (dir == NULL) return kOpenDirFailed;

    while ((file = readdir(dir)) != NULL)
    {
        file_name = file->d_name;
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        {
            continue;
        }

        if (file->d_type == DT_REG)
            files->push_back(file_name);

        // TODO: Get files recursive
        // if (file->d_type == DT_DIR)
    }

    sort(files->begin(), files->end());

    closedir(dir);

    return kOk;
}/*}}}*/

Code GetNormalFilesName(const std::string &dir_path, std::deque<std::string> *files)
{/*{{{*/
    int ret = 0;
    DIR *dir = NULL;
    struct stat st;
    struct dirent* file = NULL;
    char *file_name = NULL;

    memset(&st, 0, sizeof(struct stat));

    ret = stat(dir_path.c_str(), &st);
    if (ret == -1) return kStatFailed;

    if (!S_ISDIR(st.st_mode)) return kNotDir;

    dir = opendir(dir_path.c_str());
    if (dir == NULL) return kOpenDirFailed;

    while ((file = readdir(dir)) != NULL)
    {
        file_name = file->d_name;
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        {
            continue;
        }

        if (file->d_type == DT_REG)
            files->push_back(file_name);

        // TODO: Get files recursive
        // if (file->d_type == DT_DIR)
    }

    sort(files->begin(), files->end());

    closedir(dir);

    return kOk;
}/*}}}*/

Code GetNormalFilesPath(const std::string &dir_path, std::vector<std::string> *files)
{/*{{{*/
    int ret = 0;
    DIR *dir = NULL;
    struct stat st;
    struct dirent* file = NULL;
    char *file_name = NULL;

    memset(&st, 0, sizeof(struct stat));

    ret = stat(dir_path.c_str(), &st);
    if (ret == -1) return kStatFailed;

    if (!S_ISDIR(st.st_mode)) return kNotDir;

    dir = opendir(dir_path.c_str());
    if (dir == NULL) return kOpenDirFailed;

    while ((file = readdir(dir)) != NULL)
    {
        file_name = file->d_name;
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        {
            continue;
        }

        if (file->d_type == DT_REG)
            files->push_back(dir_path+"/"+file_name);

        // TODO: Get files recursive
        // if (file->d_type == DT_DIR)
    }

    sort(files->begin(), files->end());

    closedir(dir);

    return kOk;
}/*}}}*/

Code GetNormalFilesPath(const std::string &dir_path, std::deque<std::string> *files)
{/*{{{*/
    int ret = 0;
    DIR *dir = NULL;
    struct stat st;
    struct dirent* file = NULL;
    char *file_name = NULL;

    memset(&st, 0, sizeof(struct stat));

    ret = stat(dir_path.c_str(), &st);
    if (ret == -1) return kStatFailed;

    if (!S_ISDIR(st.st_mode)) return kNotDir;

    dir = opendir(dir_path.c_str());
    if (dir == NULL) return kOpenDirFailed;

    while ((file = readdir(dir)) != NULL)
    {
        file_name = file->d_name;
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
        {
            continue;
        }

        if (file->d_type == DT_REG)
            files->push_back(dir_path+"/"+file_name);

        // TODO: Get files recursive
        // if (file->d_type == DT_DIR)
    }

    sort(files->begin(), files->end());

    closedir(dir);

    return kOk;
}/*}}}*/

Code GetLineContentAndRemoveNewLine(const std::string &path, std::vector<std::string> *contents)
{/*{{{*/
    if (path.empty() || contents == NULL) return kInvalidParam;

    FILE *fp = fopen(path.c_str(), "r");
    if (fp == NULL) return kOpenFileFailed;

    char buf[kBufLen] = {0};

    Code ret = kOk;
    while (!feof(fp) && !ferror(fp))
    {
        memset(buf, '\0', sizeof(buf));
        char *tmp = fgets(buf, sizeof(buf), fp);
        if (tmp == NULL)
        {
            if (feof(fp)) break;

            ret = kFgetsFailed;
            break;
        }

        std::string cnt(buf);
        std::string tmp_cnt;
        ret = TrimRight(cnt, kNewLine, &tmp_cnt);
        if (ret != kOk)
        {
            LOG_ERR("Failed to trim right! ret:%d\n", ret);
            break;
        }

        std::string tmp_out_cnt;
        ret = Trim(tmp_cnt, kWhiteDelim, &tmp_out_cnt);
        if (ret != kOk)
        {
            LOG_ERR("Failed to trim right! ret:%d\n", ret);
            break;
        }

        if (tmp_out_cnt.empty() || tmp_out_cnt.data()[0] == kDefaultCommitChar)
            continue;

        contents->push_back(tmp_out_cnt);
    }

    fclose(fp);
    if (ret != kOk) return ret;

    return ret;
}/*}}}*/

}

#ifdef _FILE_UTIL_MAIN_TEST_
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;

    const std::string dir_path = "../demo/log";
    Code r = CreateDir(dir_path);
    if (r == kOk) 
        fprintf(stderr, "Dir:%s now exists\n", dir_path.c_str());
    else 
        fprintf(stderr, "Failed to create dir:%s\n", dir_path.c_str());

//    const std::string test_log_path = dir_path + "/test.log";
//    CompareAndWriteWholeFile(test_log_path, "cc");

    // Test: get files name
    std::string path = "..";
    std::vector<std::string> files;
    r= GetNormalFilesName(path, &files);
    if (r != kOk) 
    {
        fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
        return -1;
    }
    std::vector<std::string>::iterator it = files.begin();
    for (; it != files.end(); ++it)
    {
        fprintf(stderr, "%s\n", it->c_str());
    }

    // Test: get files path
    std::vector<std::string> files_path;
    r = GetNormalFilesPath(path, &files_path);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
        return -1;
    }
    for (it = files_path.begin(); it != files_path.end(); ++it)
    {
        fprintf(stderr, "%s\n", it->c_str());
    }


    fprintf(stderr, "\n");
    // test deque files
    std::deque<std::string> deque_files;
    r = GetNormalFilesName(path, &deque_files);
    if (r != kOk) 
    {
        fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
        return -1;
    }
    std::deque<std::string>::iterator deque_it = deque_files.begin();
    for (; deque_it != deque_files.end(); ++deque_it)
    {
        fprintf(stderr, "%s\n", deque_it->c_str());
    }

    // Test: get files path
    std::deque<std::string> deque_files_path;
    r = GetNormalFilesPath(path, &deque_files_path);
    if (r != kOk)
    {
        fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
        return -1;
    }
    for (deque_it = deque_files_path.begin(); deque_it != deque_files_path.end(); ++deque_it)
    {
        fprintf(stderr, "%s\n", deque_it->c_str());
    }

    fprintf(stderr, "\n");
    // Test GetLineContentAndRemoveNewLine
    path = "../demo/conf/cs.conf";
    std::vector<std::string> contents;
    r = GetLineContentAndRemoveNewLine(path, &contents);
    for (it = contents.begin(); it != contents.end(); ++it)
    {
        fprintf(stderr, "%s\n", it->c_str());
    }

    return 0;
}/*}}}*/
#endif
