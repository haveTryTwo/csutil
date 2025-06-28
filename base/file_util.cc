// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <algorithm>

#include "base/coding.h"
#include "base/common.h"
#include "base/util.h"

#if defined(__linux__) || defined(__unix__)
#  include <sys/vfs.h>
#elif defined(__APPLE__)
#  include <sys/mount.h>
#  include <sys/param.h>
#endif

namespace base {

Code CreateDir(const std::string &dir_path) { /*{{{*/
  if (dir_path.empty()) return kInvalidParam;

  int ret = access(dir_path.c_str(), F_OK);
  if (ret == 0) return kOk;

  ret = mkdir(dir_path.c_str(), kDefaultDirMode);
  if (ret != 0 && errno != EEXIST) {
#ifdef _ENABLE_SYSLOG_
    syslog(LOG_ERR, "Failed to create dir:%s, ret:%d, errno:%d", dir_path.c_str(), ret, errno);
#endif
    return kMkdirFailed;
  }

  return kOk;
} /*}}}*/

Code CompareAndWriteWholeFile(const std::string &path, const std::string &msg) { /*{{{*/
  int fd = open(path.c_str(), O_RDWR | O_CREAT, kDefaultRegularFileMode);
  if (fd == -1) return kOpenFileFailed;

  char buf[kBufLen] = {0};
  int ret = read(fd, buf, sizeof(buf));
  if (ret == -1) {
    close(fd);
    return kReadError;
  }

  if (ret == (int)msg.size() && (memcmp(buf, msg.c_str(), ret) == 0)) {
    close(fd);
    return kOk;
  }

  lseek(fd, 0, SEEK_SET);
  ftruncate(fd, 0);  // truncate the size of to 0 as the size may be larger than message
  ret = write(fd, msg.c_str(), msg.size());
  if (ret != (int)msg.size()) {
    close(fd);
    return kWriteError;
  }

  close(fd);
  return kOk;
} /*}}}*/

Code GetNormalFilesNameWithOutSort(const std::string &dir_path, std::vector<std::string> *files) { /*{{{*/
  int ret = 0;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) {
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) files->push_back(file_name);

    // TODO: Get files recursive
    // if (file->d_type == DT_DIR)
  }

  closedir(dir);

  return kOk;
} /*}}}*/

Code GetNormalFilesNameWithOutSort(const std::string &dir_path, std::deque<std::string> *files) { /*{{{*/
  int ret = 0;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) {
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) files->push_back(file_name);

    // TODO: Get files recursive
    // if (file->d_type == DT_DIR)
  }

  closedir(dir);

  return kOk;
} /*}}}*/

Code GetNormalFilesName(const std::string &dir_path, std::vector<std::string> *files) { /*{{{*/
  Code ret = GetNormalFilesNameWithOutSort(dir_path, files);
  if (ret != kOk) return ret;

  std::sort(files->begin(), files->end());

  return kOk;
} /*}}}*/

Code GetNormalFilesName(const std::string &dir_path, std::deque<std::string> *files) { /*{{{*/
  Code ret = GetNormalFilesNameWithOutSort(dir_path, files);
  if (ret != kOk) return ret;

  std::sort(files->begin(), files->end());

  return kOk;
} /*}}}*/

Code GetNormalFilesPathWithOutSort(const std::string &dir_path, std::vector<std::string> *files) { /*{{{*/
  int ret = 0;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) {
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) files->push_back(dir_path + "/" + file_name);

    // TODO: Get files recursive
    // if (file->d_type == DT_DIR)
  }

  closedir(dir);

  return kOk;
} /*}}}*/

Code GetNormalFilesPathRecurWithOutSort(const std::string &dir_path, std::vector<std::string> *files) { /*{{{*/
  int ret = 0;
  Code r = kOk;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) { /*{{{*/
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) files->push_back(dir_path + "/" + file_name);

    // Get files recursive
    if (file->d_type == DT_DIR) {
      r = GetNormalFilesPathRecurWithOutSort(dir_path + "/" + file_name, files);
      if (r != kOk) break;
    }
  } /*}}}*/

  closedir(dir);

  return r;
} /*}}}*/

Code GetNormalFilesPathWithOutSort(const std::string &dir_path, std::deque<std::string> *files) { /*{{{*/
  int ret = 0;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) {
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) files->push_back(dir_path + "/" + file_name);

    // TODO: Get files recursive
    // if (file->d_type == DT_DIR)
  }

  closedir(dir);

  return kOk;
} /*}}}*/

Code GetNormalFilesPath(const std::string &dir_path, std::vector<std::string> *files) { /*{{{*/
  Code ret = GetNormalFilesPathWithOutSort(dir_path, files);
  if (ret != kOk) return ret;

  std::sort(files->begin(), files->end());

  return kOk;
} /*}}}*/

Code GetNormalFilesPath(const std::string &dir_path, std::deque<std::string> *files) { /*{{{*/
  Code ret = GetNormalFilesPathWithOutSort(dir_path, files);
  if (ret != kOk) return ret;

  std::sort(files->begin(), files->end());

  return kOk;
} /*}}}*/

Code GetNormalFilesPathWithOutSort(const std::vector<std::string> &dirs_path, std::vector<std::string> *files) { /*{{{*/
  if (files == NULL) return kInvalidParam;

  Code ret = kOk;
  std::vector<std::string>::const_iterator it;
  for (it = dirs_path.begin(); it != dirs_path.end(); ++it) {
    ret = GetNormalFilesPathWithOutSort(*it, files);
    if (ret != kOk) return ret;
  }

  return ret;

} /*}}}*/

Code GetNormalFilesPath(const std::vector<std::string> &dirs_path, std::vector<std::string> *files) { /*{{{*/
  Code ret = GetNormalFilesPathWithOutSort(dirs_path, files);
  if (ret != kOk) return ret;

  std::sort(files->begin(), files->end());

  return kOk;
} /*}}}*/

Code GetNormalFilesPathWithOutSort(const std::string &src_dir_path, std::vector<std::string> *src_files,
                                   const std::string &dst_dir_path, std::vector<std::string> *dst_files) { /*{{{*/
  int ret = 0;
  DIR *dir = NULL;
  struct stat st;
  struct dirent *file = NULL;
  char *file_name = NULL;

  memset(&st, 0, sizeof(struct stat));

  ret = stat(src_dir_path.c_str(), &st);
  if (ret == -1) return kStatFailed;

  if (!S_ISDIR(st.st_mode)) return kNotDir;

  dir = opendir(src_dir_path.c_str());
  if (dir == NULL) return kOpenDirFailed;

  while ((file = readdir(dir)) != NULL) {
    file_name = file->d_name;
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
      continue;
    }

    if (file->d_type == DT_REG) {
      src_files->push_back(src_dir_path + "/" + file_name);
      dst_files->push_back(dst_dir_path + "/" + file_name);
    }

    // TODO: Get files recursive
    // if (file->d_type == DT_DIR)
  }

  closedir(dir);

  return kOk;
} /*}}}*/

Code GetLineContentAndRemoveNewLine(const std::string &path, std::vector<std::string> *contents) { /*{{{*/
  if (path.empty() || contents == NULL) return kInvalidParam;

  FILE *fp = fopen(path.c_str(), "r");
  if (fp == NULL) return kOpenFileFailed;

  char buf[kBufLen] = {0};

  Code ret = kOk;
  while (!feof(fp) && !ferror(fp)) {
    memset(buf, '\0', sizeof(buf));
    char *tmp = fgets(buf, sizeof(buf), fp);
    if (tmp == NULL) {
      if (feof(fp)) break;

      ret = kFgetsFailed;
      break;
    }

    std::string cnt(buf);
    std::string tmp_cnt;
    ret = TrimRight(cnt, kNewLine, &tmp_cnt);
    if (ret != kOk) {
      syslog(LOG_ERR, "Failed to trim right! ret:%d", ret);
      break;
    }

    std::string tmp_out_cnt;
    ret = Trim(tmp_cnt, kWhiteDelim, &tmp_out_cnt);
    if (ret != kOk) {
      syslog(LOG_ERR, "Failed to trim right! ret:%d", ret);
      break;
    }

    if (tmp_out_cnt.empty() || tmp_out_cnt.data()[0] == kDefaultCommitChar) continue;

    contents->push_back(tmp_out_cnt);
  }

  fclose(fp);
  if (ret != kOk) return ret;

  return ret;
} /*}}}*/

Code GetFileSize(int fd, uint64_t *file_size) { /*{{{*/
  if (file_size == NULL) return kInvalidParam;

  struct stat st;
  int ret = fstat(fd, &st);
  if (ret != 0) return kStatFailed;
  *file_size = st.st_size;

  return kOk;
} /*}}}*/

Code GetFileSize(const std::string &file_path, uint64_t *file_size) { /*{{{*/
  if (file_size == NULL) return kInvalidParam;

  struct stat st;
  int ret = stat(file_path.c_str(), &st);
  if (ret != 0) return kStatFailed;
  *file_size = st.st_size;

  return kOk;
} /*}}}*/

Code GetFilesSize(const std::vector<std::string> &files_path, uint64_t *files_size) { /*{{{*/
  if (files_size == NULL) return kInvalidParam;
  *files_size = 0;

  Code r = kOk;
  std::vector<std::string>::const_iterator const_it;
  for (const_it = files_path.begin(); const_it != files_path.end(); ++const_it) {
    uint64_t file_size = 0;
    r = GetFileSize(*const_it, &file_size);
    if (r != kOk) break;

    *files_size += file_size;
  }

  return r;
} /*}}}*/

Code GetFilesSize(const std::string &dir_path, uint64_t *files_size) { /*{{{*/
  std::vector<std::string> files_path;
  Code r = GetNormalFilesPath(dir_path, &files_path);
  if (r != kOk) return r;

  r = GetFilesSize(files_path, files_size);
  if (r != kOk) return r;

  return r;
} /*}}}*/

Code GetFileSystemSize(const std::string &sys_path, uint64_t *total_size, uint64_t *free_size) { /*{{{*/
  if (total_size == NULL || free_size == NULL) return kInvalidParam;

  struct statfs stfs;
  int ret = statfs(sys_path.c_str(), &stfs);
  if (ret != 0) return kStatfsFailed;
  *total_size = stfs.f_bsize * stfs.f_blocks;
  *free_size = stfs.f_bsize * stfs.f_bfree;

  return kOk;
} /*}}}*/

Code CheckFileExist(const std::string &file_path, bool *is_exist) { /*{{{*/
  if (is_exist == NULL) return kInvalidParam;

  *is_exist = false;
  int ret = access(file_path.c_str(), F_OK);
  if (ret == 0) {
    *is_exist = true;
  }

  return kOk;
} /*}}}*/

Code MoveFile(const std::string &old_path, const std::string &new_path) { /*{{{*/
  if (old_path.empty() || new_path.empty()) return kInvalidParam;

  if (old_path.compare(new_path) == 0) return kOk;

  int ret = rename(old_path.c_str(), new_path.c_str());
  if (ret != 0) return kRenameFailed;

  return kOk;
} /*}}}*/

Code DumpBinData(const std::string &bin_str, FILE *fp) { /*{{{*/
  if (bin_str.empty() || fp == NULL) return kInvalidParam;

  std::string data_len;
  Code r = EncodeFixed32(bin_str.size(), &data_len);
  if (r != kOk) return r;

  // merge data_len and bin_str
  data_len += bin_str;
  int ret_len = fwrite(data_len.data(), sizeof(char), data_len.size(), fp);
  if (ret_len != (int)data_len.size()) return kWriteError;

  return kOk;
} /*}}}*/

Code PumpBinData(std::string *bin_str, FILE *fp) { /*{{{*/
  if (bin_str == NULL || fp == NULL) return kInvalidParam;

  if (ferror(fp)) return kReadError;
  if (feof(fp)) return kFileIsEnd;

  bin_str->clear();

  char buf[kBufLen];
  memset(buf, '\0', sizeof(buf));

  int ret_len = fread(buf, sizeof(char), kHeadLen, fp);
  if (ret_len != kHeadLen) {
    if (ret_len == 0 && feof(fp)) return kFileIsEnd;
    return kReadError;
  }

  uint32_t data_len = 0;
  Code r = DecodeFixed32(std::string(buf, kHeadLen), &data_len);
  if (r != kOk) return r;

  int left_len = (int)data_len;
  while (left_len > 0) {
    int read_len = left_len < (int)sizeof(buf) ? left_len : (int)sizeof(buf);

    ret_len = fread(buf, sizeof(char), read_len, fp);
    if (ret_len != read_len) return kReadError;

    bin_str->append(buf, ret_len);

    left_len -= ret_len;
  }

  return kOk;
} /*}}}*/

Code DumpStringData(const std::string &str, FILE *fp) { /*{{{*/
  if (str.empty() || fp == NULL) return kInvalidParam;

  fputs(str.c_str(), fp);

  return kOk;
} /*}}}*/

Code DumpWholeData(const std::string &str, FILE *fp) { /*{{{*/
  if (str.empty() || fp == NULL) return kInvalidParam;

  // merge data_len and bin_str
  int ret_len = fwrite(str.data(), sizeof(char), str.size(), fp);
  if (ret_len != (int)str.size()) return kWriteError;

  return kOk;
} /*}}}*/

Code DumpWholeData(const std::string &path, const std::string &cnt_str) { /*{{{*/
  if (cnt_str.empty() || path.empty()) return kInvalidParam;

  FILE *fp = fopen(path.c_str(), "w+");
  if (fp == NULL) return kOpenFileFailed;

  Code ret = DumpWholeData(cnt_str, fp);
  fclose(fp);

  return ret;
} /*}}}*/

Code PumpStringData(std::string *str, FILE *fp) { /*{{{*/
  if (str == NULL || fp == NULL) return kInvalidParam;

  if (ferror(fp)) return kReadError;
  if (feof(fp)) return kFileIsEnd;

  std::string tmp;
  char buf[kBufLen];
  while (true) {
    memset(buf, '\0', sizeof(buf));
    char *p = fgets(buf, sizeof(buf), fp);
    if (p == NULL) {
      if (feof(fp)) {
        if (!tmp.empty()) break;

        return kFileIsEnd;
      }

      return kReadError;
    }

    tmp += buf;

    if (strchr(buf, kNewLine) != NULL) break;
  }

  str->assign(tmp);

  return kOk;
} /*}}}*/

Code PumpWholeData(std::string *cnt_str, FILE *fp) { /*{{{*/
  if (cnt_str == NULL || fp == NULL) return kInvalidParam;

  if (ferror(fp)) return kReadError;
  if (feof(fp)) return kFileIsEnd;

  cnt_str->clear();

  int fd = fileno(fp);
  uint64_t file_size = 0;
  Code ret = GetFileSize(fd, &file_size);
  if (ret != kOk) return ret;

  // NOTE:htt, Avoid reading the entire file too large, resulting in memory exceptions
  if (file_size > kMaxReadBufLen) {
    return kReadFileLenExtendMax;
  }

  char buf[kBufLen];
  memset(buf, '\0', sizeof(buf));

  int left_len = (int)file_size;
  while (left_len > 0) {
    int read_len = left_len < (int)sizeof(buf) ? left_len : (int)sizeof(buf);

    size_t ret_len = fread(buf, sizeof(char), read_len, fp);
    if ((int)ret_len != read_len) return kReadError;

    cnt_str->append(buf, ret_len);

    left_len -= ret_len;
  }

  return kOk;
} /*}}}*/

Code PumpWholeData(const std::string &path, std::string *cnt_str) { /*{{{*/
  if (cnt_str == NULL || path.empty()) return kInvalidParam;

  FILE *fp = fopen(path.c_str(), "r");
  if (fp == NULL) return kOpenFileFailed;

  Code ret = PumpWholeData(cnt_str, fp);
  fclose(fp);

  return ret;
} /*}}}*/

Code ReplaceFileContent(const std::string &file_path, uint64_t replace_pos, uint64_t replace_len,
                        const std::string &replace_str) { /*{{{*/
  if (file_path.empty() || replace_str.empty()) return kInvalidParam;
  if (replace_len != replace_str.size()) return kInvalidParam;

  FILE *fp = fopen(file_path.c_str(), "r+");
  if (fp == NULL) return kOpenFileFailed;

  uint64_t file_size = 0;
  Code ret = GetFileSize(file_path, &file_size);
  if (ret != kOk) {
    fclose(fp);
    return ret;
  }
  if (replace_pos + replace_len > file_size) {
    fclose(fp);
    return kInvalidParam;
  }

  fseek(fp, replace_pos, SEEK_SET);
  uint64_t left_len = replace_len;
  while (left_len > 0) {
    uint64_t would_write_len = left_len < (uint64_t)kBufLen ? left_len : kBufLen;
    size_t ret_len = fwrite(replace_str.data() + replace_str.size() - left_len, sizeof(char), would_write_len, fp);
    if (ret_len != would_write_len) return kWriteError;

    left_len -= would_write_len;
  }

  fclose(fp);
  fp = NULL;

  return ret;
} /*}}}*/

}  // namespace base

#ifdef _FILE_UTIL_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
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
  r = GetNormalFilesName(path, &files);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
    return -1;
  }
  std::vector<std::string>::iterator it = files.begin();
  for (; it != files.end(); ++it) {
    fprintf(stderr, "%s\n", it->c_str());
  }

  // Test: get files path
  std::vector<std::string> files_path;
  r = GetNormalFilesPath(path, &files_path);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
    return -1;
  }
  uint64_t total_size = 0;
  for (it = files_path.begin(); it != files_path.end(); ++it) {
    uint64_t file_size = 0;
    r = GetFileSize(it->c_str(), &file_size);
    fprintf(stderr, "file:%s size:%llu\n", it->c_str(), file_size);
    total_size += file_size;
  }

  // Test: get files path of more directories
  std::vector<std::string> dirs_path;
  dirs_path.push_back("../sock");
  dirs_path.push_back("../cs");
  fprintf(stderr, "\nGet files of many directories:\n");
  files_path.clear();
  r = GetNormalFilesPathWithOutSort(dirs_path, &files_path);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! ret:%d\n", (int)r);
    return -1;
  }
  total_size = 0;
  for (it = files_path.begin(); it != files_path.end(); ++it) {
    uint64_t file_size = 0;
    r = GetFileSize(it->c_str(), &file_size);
    fprintf(stderr, "file:%s size:%llu\n", it->c_str(), file_size);
    total_size += file_size;
  }

  // Test: get files path in src_dir, and setting the same name in dst_dir
  std::string src_dir_path = "../cs";
  std::vector<std::string> src_files_path;
  std::string dst_dir_path = "../dst";
  std::vector<std::string> dst_files_path;
  fprintf(stderr, "\nGet files path in src_dir, and setting the same name in dst_dir:\n");
  r = GetNormalFilesPathWithOutSort(src_dir_path, &src_files_path, dst_dir_path, &dst_files_path);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! ret:%d\n", (int)r);
    return -1;
  }
  std::vector<std::string>::iterator dst_it;
  for (it = src_files_path.begin(), dst_it = dst_files_path.begin();
       it != src_files_path.end() && dst_it != dst_files_path.end(); ++it, ++dst_it) {
    fprintf(stderr, "src_file:%s => dst_file:%s\n", it->c_str(), dst_it->c_str());
  }

  // Test Get file size
  uint64_t files_size = 0;
  r = GetFilesSize(files_path, &files_size);
  fprintf(stderr, "\nTotal size:%llu, total_size:%llu\n", files_size, total_size);

  r = GetFilesSize(path, &files_size);
  fprintf(stderr, "dir:%s has files size:%llu\n", path.c_str(), files_size);

  fprintf(stderr, "\n");

  fprintf(stderr, "\n");
  // test deque files
  std::deque<std::string> deque_files;
  r = GetNormalFilesName(path, &deque_files);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
    return -1;
  }
  std::deque<std::string>::iterator deque_it = deque_files.begin();
  for (; deque_it != deque_files.end(); ++deque_it) {
    fprintf(stderr, "%s\n", deque_it->c_str());
  }

  // Test: get files path
  std::deque<std::string> deque_files_path;
  r = GetNormalFilesPath(path, &deque_files_path);
  if (r != kOk) {
    fprintf(stderr, "Failed to get normal files! path:%s, ret:%d\n", path.c_str(), (int)r);
    return -1;
  }
  for (deque_it = deque_files_path.begin(); deque_it != deque_files_path.end(); ++deque_it) {
    fprintf(stderr, "%s\n", deque_it->c_str());
  }

  fprintf(stderr, "\n");
  // Test GetLineContentAndRemoveNewLine
  path = "../demo/conf/cs.conf";
  std::vector<std::string> contents;
  r = GetLineContentAndRemoveNewLine(path, &contents);
  for (it = contents.begin(); it != contents.end(); ++it) {
    fprintf(stderr, "%s\n", it->c_str());
  }

  std::string old_path = "build.bak";
  std::string new_path = "build";
  r = MoveFile(old_path, new_path);
  if (r == kOk) {
    fprintf(stderr, "[Success] move file:%s to %s success!\n", old_path.c_str(), new_path.c_str());
  } else {
    fprintf(stderr, "[Failed] move file:%s to %s failed!\n", old_path.c_str(), new_path.c_str());
  }

  // Test DumpBinData and PumpBinData
  fprintf(stderr, "\n");
  std::string dump_path = "../demo/log/dump.txt";
  std::string bin_data = "123abcdefghijklmn";
  FILE *fp = fopen(dump_path.c_str(), "w+");
  if (fp == NULL) return kOpenError;
  r = DumpBinData(bin_data, fp);
  if (r != kOk) {
    fprintf(stderr, "Failed to dump bin data! ret:%d\n", r);
    fclose(fp);
    return r;
  }
  fclose(fp);

  fp = fopen(dump_path.c_str(), "r");
  if (fp == NULL) return kOpenError;
  while (true) {
    std::string bin_data_pump;
    r = PumpBinData(&bin_data_pump, fp);
    if (r == kFileIsEnd) {
      fprintf(stderr, "Read over\n");
      break;
    }

    if (r != kOk) {
      fprintf(stderr, "Failed to pump bin data! ret:%d\n", r);
      fclose(fp);
      return r;
    }
    fprintf(stderr, "bin_data_dump:%s\n", bin_data.c_str());
    fprintf(stderr, "bin_data_pump:%s\n", bin_data_pump.c_str());
  }
  fclose(fp);
  fp = NULL;

  // Test DumpBinData and PumpBinData
  fprintf(stderr, "\n");
  std::string dump_line_path = "../demo/log/dump_line.txt";
  FILE *line_fp = fopen(dump_line_path.c_str(), "w+");
  if (line_fp == NULL) return kOpenError;

  std::string str_data = "abcdefghijklmn\n";
  r = DumpStringData(str_data, line_fp);

  str_data = "123456\n";
  r = DumpStringData(str_data, line_fp);

  str_data.assign(4096, 'a');
  str_data.append(2, 'b');
  str_data.append(2, 'c');
  str_data.append(1, '\n');
  r = DumpStringData(str_data, line_fp);

  str_data = "mmm";
  r = DumpStringData(str_data, line_fp);

  if (r != kOk) {
    fprintf(stderr, "Failed to dump bin data! ret:%d\n", r);
    fclose(line_fp);
    return r;
  }
  fclose(line_fp);

  line_fp = fopen(dump_line_path.c_str(), "r");
  if (line_fp == NULL) return kOpenError;
  while (true) {
    std::string str_data_pump;
    r = PumpStringData(&str_data_pump, line_fp);
    if (r == kFileIsEnd) {
      fprintf(stderr, "Read over\n");
      break;
    }

    if (r != kOk) {
      fprintf(stderr, "Failed to pump str data! ret:%d\n", r);
      fclose(line_fp);
      return r;
    }
    fprintf(stderr, "str_data_pump:%s\n", str_data_pump.c_str());
  }
  fclose(line_fp);
  line_fp = NULL;

  std::string replace_str = "zhangsan";
  std::string would_replace_file = "../demo/log/would_replace.txt";
  uint64_t replace_pos = 5;
  r = ReplaceFileContent(would_replace_file, replace_pos, replace_str.size(), replace_str);
  if (r == kOk) {
    fprintf(stderr, "[SUCC] replace file:%s start pos:%llu, and start str:%s\n", would_replace_file.c_str(),
            replace_pos, replace_str.c_str());
  } else {
    fprintf(stderr, "[FAILED] replace file:%s start pos:%llu, and start str:%s\n", would_replace_file.c_str(),
            replace_pos, replace_str.c_str());
  }

#  ifdef _ENABLE_SYSLOG_
  syslog(LOG_ERR, "Failed to test!");
#  endif

  return 0;
} /*}}}*/
#endif
