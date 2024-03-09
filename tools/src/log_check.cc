// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "base/common.h"
#include "base/file_util.h"
#include "base/util.h"

#include "log_check.h"

namespace tools {

/**
 *  Note:
 *    1. %% should not be considered as placeholder, and as the same of other even numbers of %
 *    2. %1111% should not be considered as placeholder
 */
base::Code GetSizeOfNotPlaceholder(const char *str, uint32_t size, uint32_t *ignore_size) { /*{{{*/
  if (str == NULL || ignore_size == NULL) return base::kInvalidParam;

  *ignore_size = 0;
  if (size == 0 || str[0] != base::kPercent) return base::kOk;

  uint32_t num_of_percent = 0;
  base::Code ret = base::kOk;
  for (uint32_t i = 0; i < size; ++i) {
    if (str[i] == base::kPercent) {
      ++num_of_percent;

      if (num_of_percent % 2 == 0) *ignore_size = i;

      continue;
    }

    if (isdigit(str[i])) continue;

    break;
  }

  return ret;
} /*}}}*/

/**
 * Note:
 *  1. \" should be ignored as it's not double quotes
 *  2. \\" should not be ignored as BackSlash is escaped
 */
base::Code GetSizeOfBackSlash(const char *str, uint32_t size, uint32_t *ignore_size) { /*{{{*/
  if (str == NULL || ignore_size == NULL) return base::kInvalidParam;

  *ignore_size = 0;
  if (size == 0 || str[0] != base::kBackSlach) return base::kOk;

  uint32_t num_of_back_slach = 0;
  base::Code ret = base::kOk;
  for (uint32_t i = 0; i < size; ++i) {
    if (str[i] == base::kBackSlach) {
      ++num_of_back_slach;
      continue;
    } else if (str[i] == base::kDoubleQuotes) {
      assert(i > 0);
      if (num_of_back_slach % 2 == 0)
        *ignore_size = i - 1;
      else
        *ignore_size = i;
    } else {
      assert(i > 0);
      *ignore_size = i - 1;
    }

    break;
  }

  return ret;
} /*}}}*/

base::Code CheckLogFormat(const std::string &cnt, bool *is_satisfied) { /*{{{*/
  if (cnt.empty() || is_satisfied == NULL) return base::kInvalidParam;

  *is_satisfied = false;

  base::Code ret = base::kOk;

  int num_of_percent = 0;
  int num_of_comma = 0;
  int num_of_parentheses_in_using = 0;
  bool has_parentheses_flag = false;
  bool is_double_quotes_found = false;
  bool is_double_quotes_finish = false;

  // double quote maybe in data for printing just as printf("%s", "aa,bb")
  bool is_data_double_quote_found = false;

  char comma = ',';
  char back_slash = '\\';
  char double_quotes = '"';
  char left_parentheses = '(';
  char right_parentheses = ')';
  uint32_t ignore_size = 0;

  for (int i = 0; i < (int)cnt.size(); ++i) { /*{{{*/
    if (!is_double_quotes_found) {
      if (cnt.data()[i] != double_quotes) continue;

      is_double_quotes_found = true;
      continue;
    }

    if (!is_double_quotes_finish) {
      if (cnt.data()[i] != double_quotes) {
        if (cnt.data()[i] != base::kPercent) {
          // Note: check back slach and double quote
          if (cnt.data()[i] == back_slash) {
            ret = GetSizeOfBackSlash(cnt.data() + i, cnt.size() - i, &ignore_size);
            assert(ret == base::kOk);
            i += ignore_size;  // Note: The way to change 'i' is not recommended
          }

          continue;
        }

        ret = GetSizeOfNotPlaceholder(cnt.data() + i, cnt.size() - i, &ignore_size);
        assert(ret == base::kOk);

        if (ignore_size == 0)
          ++num_of_percent;
        else
          i += ignore_size;  // Note: The way to change 'i' is not recommended

        continue;
      } else {
        // Note: BackSlash has been checked
        is_double_quotes_finish = true;
        continue;
      }
    }

    // new double quotes line
    if (num_of_comma == 0 && !has_parentheses_flag && cnt.data()[i] == double_quotes) {
      is_double_quotes_finish = false;
      continue;
    }

    // check for data double quotes
    if (num_of_comma > 0 && !is_data_double_quote_found && cnt.data()[i] == double_quotes) {
      is_data_double_quote_found = true;
      continue;
    }

    if (is_data_double_quote_found) {
      if (cnt.data()[i] == double_quotes) {
        // Note: back_slash has been check
        is_data_double_quote_found = false;
        continue;
      } else if (cnt.data()[i] == back_slash) {
        // Note: check back slach and double quote
        ret = GetSizeOfBackSlash(cnt.data() + i, cnt.size() - i, &ignore_size);
        assert(ret == base::kOk);
        i += ignore_size;  // Note: The way to change 'i' is not recommended

        continue;
      }

      continue;
    }

    if (cnt.data()[i] == left_parentheses) {
      ++num_of_parentheses_in_using;
      has_parentheses_flag = true;
      continue;
    }

    if (cnt.data()[i] == right_parentheses) {
      --num_of_parentheses_in_using;
      continue;
    }

    if (num_of_parentheses_in_using == 0 && cnt.data()[i] == comma) {
      ++num_of_comma;
      continue;
    }
  } /*}}}*/

  if (num_of_percent == num_of_comma) {
    *is_satisfied = true;
  } else {
    fprintf(stderr, "num_of_percent:%d, num_of_comma:%d\n", num_of_percent, num_of_comma);
  }

  return ret;
} /*}}}*/

/**
 * NOTE:htt, 判断是否有注释符合
 * 说明：一般不会在打印日志行的末尾追加注释
 *
 * TODO:htt, 1. 针对注释内容有"的处理: "%d" // "
 *           2. 针对 slash* *slash 注释处理
 */
static base::Code CheckHasComment(const std::string &cnt, bool *has_comment,
                                  int *comment_start_pos) { /*{{{*/
  if (has_comment == NULL || comment_start_pos == NULL) return base::kInvalidParam;

  *has_comment = false;
  *comment_start_pos = 0;
  for (int i = static_cast<int>(cnt.size()) - 1; i >= 0; --i) {
    // NOTE:htt, 如果有双引号，暂时认为//在""内，后续可以进一步优化
    if (cnt.data()[i] == base::kDoubleQuotes) break;

    if (cnt.data()[i] == base::kSlash) {
      if (i == 0) break;

      if (cnt.data()[i - 1] == base::kSlash) {  // NOTE:htt, 有注释符号
        *has_comment = true;
        *comment_start_pos = i - 1;
        return base::kOk;
      }
    }
  }

  return base::kOk;
} /*}}}*/

static base::Code CheckIsSemicolonAfterTrim(const std::string &cnt, bool *is_finish) { /*{{{*/
  if (is_finish == NULL) return base::kOk;

  *is_finish = false;

  std::string right_delims = " \t\r\n";
  std::string out_str;
  base::Code ret = base::TrimRight(cnt, right_delims, &out_str);
  if (ret != base::kOk) return ret;

  if (out_str.size() == 0) return base::kOk;
  if (out_str.data()[out_str.size() - 1] == base::kSemicolon) {
    *is_finish = true;
  }

  return base::kOk;
} /*}}}*/

static base::Code CheckWholeLineAppend(const std::string part_line, bool has_comment,
                                       int comment_start_pos, std::string *whole_line) { /*{{{*/
  if (whole_line == NULL) return base::kInvalidParam;

  if (!has_comment) {
    whole_line->append(part_line);
  } else {
    if (comment_start_pos != 0) {
      whole_line->append(part_line.substr(0, comment_start_pos));
    }
  }

  return base::kOk;
} /*}}}*/

static base::Code CheckPartLineIsFinish(const std::string &part_line, bool has_comment,
                                        int comment_start_pos, bool *is_finish) { /*{{{*/
  if (is_finish == NULL) return base::kInvalidParam;

  *is_finish = false;
  if (part_line.size() == 0) return base::kOk;

  base::Code ret = base::kOk;
  if (!has_comment) {
    ret = CheckIsSemicolonAfterTrim(part_line, is_finish);
    return ret;
  }

  // NOTE:htt, 在日志中添加一整行为注释，应很不常见
  if (comment_start_pos == 0) return base::kOk;

  std::string real_cnt = part_line.substr(0, comment_start_pos);
  ret = CheckIsSemicolonAfterTrim(real_cnt, is_finish);
  return ret;
} /*}}}*/

static base::Code CheckWholeLineIsFinish(const std::string &part_line, std::string *whole_line,
                                         bool *is_finish) { /*{{{*/
  if (whole_line == NULL || is_finish == NULL) return base::kInvalidParam;

  *is_finish = false;

  bool has_comment = false;
  int comment_start_pos = 0;
  base::Code ret = CheckHasComment(part_line, &has_comment, &comment_start_pos);
  if (ret != base::kOk) return ret;

  ret = CheckWholeLineAppend(part_line, has_comment, comment_start_pos, whole_line);
  if (ret != base::kOk) return ret;

  ret = CheckPartLineIsFinish(part_line, has_comment, comment_start_pos, is_finish);
  return ret;
} /*}}}*/

base::Code CheckLogFormat(const std::string &path, const std::string &log_name,
                          std::map<uint32_t, bool> *result) { /*{{{*/
  if (path.empty() || log_name.empty() || result == NULL) return base::kInvalidParam;

  FILE *fp = fopen(path.c_str(), "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", path.c_str());
    return base::kOpenFileFailed;
  }

  fprintf(stderr, "[BEGIN] Start to read file:%s\n", path.c_str());

  base::Code ret = base::kOk;
  uint32_t line = 0;
  std::string cnt;
  uint32_t new_log_line = 0;
  bool is_new_log_line = false;
  while (true) { /*{{{*/
    ++line;
    std::string tmp_cnt;
    ret = base::PumpStringData(&tmp_cnt, fp);
    if (ret != base::kOk) { /*{{{*/
      if (ret == base::kFileIsEnd) {
        --line;
        fprintf(stderr, "[END] File:%s has been finished!\n", path.c_str());
        ret = base::kOk;
        break;
      } else {
        fprintf(stderr, "[FAILED] Reading file:%s failed!\n", path.c_str());
        break;
      }
    } /*}}}*/

    if (!is_new_log_line) { /*{{{*/
      std::string left_delims = " \t";
      std::string out_str;
      ret = base::TrimLeft(tmp_cnt, left_delims, &out_str);
      assert(ret == base::kOk);

      if (out_str.compare(0, log_name.size(), log_name) != 0) continue;

      is_new_log_line = true;
      new_log_line = line;
    } /*}}}*/

    bool is_finish = false;
    ret = CheckWholeLineIsFinish(tmp_cnt, &cnt, &is_finish);
    if (ret != base::kOk) return ret;
    if (!is_finish) {
      continue;
    }

    // check log content
    bool is_satisfied = false;
    ret = CheckLogFormat(cnt, &is_satisfied);
    if (ret != base::kOk) return ret;

    if (is_satisfied) {
      result->insert(std::pair<uint32_t, bool>(new_log_line, true));
    } else {
      fprintf(stderr, "[NOT RIGHT] check log format failed, Line:%u, cnt:\n%s\n", new_log_line,
              cnt.c_str());
      result->insert(std::pair<uint32_t, bool>(new_log_line, false));
    }

    is_new_log_line = false;
    new_log_line = 0;
    cnt.clear();
  } /*}}}*/

  fclose(fp);
  fp = NULL;

  return ret;
} /*}}}*/

base::Code CheckLogFormatForCC(const std::string &path, const std::string &log_name) { /*{{{*/
  if (path.empty() || log_name.empty()) return base::kInvalidParam;

  std::map<uint32_t, bool> result;
  base::Code ret = CheckLogFormat(path, log_name, &result);
  assert(ret == base::kOk);

  std::map<uint32_t, bool>::iterator map_it;
  uint32_t num_of_satisfied = 0;
  uint32_t num_of_not_satisfied = 0;
  for (map_it = result.begin(); map_it != result.end(); ++map_it) {
    if (map_it->second)
      ++num_of_satisfied;
    else
      ++num_of_not_satisfied;
  }

  assert(result.size() == num_of_satisfied + num_of_not_satisfied);
  fprintf(stderr, "File:%s, total log line:%zu, right line:%u, not right line:%u\n", path.c_str(),
          result.size(), num_of_satisfied, num_of_not_satisfied);

  return ret;
} /*}}}*/

base::Code CheckLogFormatForCCS(const std::string &dir, const std::string &log_name) { /*{{{*/
  if (dir.empty() || log_name.empty()) return base::kInvalidParam;

  std::vector<std::string> files_path;
  base::Code ret = base::GetNormalFilesPathRecurWithOutSort(dir, &files_path);
  assert(ret == base::kOk);

  uint32_t total_num_of_satisfied = 0;
  uint32_t total_num_of_not_satisfied = 0;
  std::vector<std::string>::iterator vec_it;
  for (vec_it = files_path.begin(); vec_it != files_path.end(); ++vec_it) {
    bool is_cpp_flags = false;
    ret = base::CheckIsCplusplusFile(*vec_it, &is_cpp_flags);
    assert(ret == base::kOk);
    if (!is_cpp_flags) continue;

    std::map<uint32_t, bool> result;
    ret = CheckLogFormat(*vec_it, log_name, &result);
    assert(ret == base::kOk);

    std::map<uint32_t, bool>::iterator map_it;
    uint32_t num_of_satisfied = 0;
    uint32_t num_of_not_satisfied = 0;
    for (map_it = result.begin(); map_it != result.end(); ++map_it) {
      if (map_it->second)
        ++num_of_satisfied;
      else
        ++num_of_not_satisfied;
    }

    assert(result.size() == num_of_satisfied + num_of_not_satisfied);
    fprintf(stderr, "File:%s, total log line:%zu, right line:%u, not right line:%u\n\n",
            vec_it->c_str(), result.size(), num_of_satisfied, num_of_not_satisfied);

    total_num_of_satisfied += num_of_satisfied;
    total_num_of_not_satisfied += num_of_not_satisfied;
  }

  fprintf(stderr, "\ndir:%s, total log num:%u, right line:%u, not right line:%u\n", dir.c_str(),
          total_num_of_satisfied + total_num_of_not_satisfied, total_num_of_satisfied,
          total_num_of_not_satisfied);

  return ret;
} /*}}}*/

base::Code LogContent(const std::string &path, const std::string &cnt_key,
                      int log_interval_logs) { /*{{{*/
  if (path.empty() || cnt_key.empty() || log_interval_logs < 0 || log_interval_logs > 10)
    return base::kInvalidParam;

  FILE *fp = fopen(path.c_str(), "r");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", path.c_str());
    return base::kOpenFileFailed;
  }

  fprintf(stderr, "[BEGIN] Start to read file:%s\n", path.c_str());

  base::Code ret = base::kOk;
  uint32_t line = 0;
  std::string cnt;
  int cur_interval = 0;
  bool is_start_log = false;
  while (true) { /*{{{*/
    line++;
    std::string tmp_cnt;
    ret = base::PumpStringData(&tmp_cnt, fp);
    if (ret != base::kOk) { /*{{{*/
      if (ret == base::kFileIsEnd) {
        fprintf(stderr, "[END] File:%s has been finished!\n", path.c_str());
        ret = base::kOk;
        break;
      } else {
        fprintf(stderr, "[FAILED] Reading file:%s failed!\n", path.c_str());
        break;
      }
    } /*}}}*/

    if (!is_start_log) {
      if (strstr(tmp_cnt.c_str(), cnt_key.c_str()) != NULL) {
        fprintf(stderr, "line:%d, %s", line, tmp_cnt.c_str());
        is_start_log = true;
        ++cur_interval;
      }
    } else {
      if (cur_interval <= log_interval_logs) {
        fprintf(stderr, "line:%d, %s", line, tmp_cnt.c_str());
        ++cur_interval;
      } else {
        is_start_log = false;
        cur_interval = 0;
      }
    }
  } /*}}}*/

  fclose(fp);
  fp = NULL;

  return ret;
} /*}}}*/

base::Code LogContentInDir(const std::string &dir, const std::string &cnt_key,
                           int log_interval_logs) { /*{{{*/
  if (dir.empty() || cnt_key.empty() || log_interval_logs < 0 || log_interval_logs > 10)
    return base::kInvalidParam;

  std::vector<std::string> files_path;
  base::Code ret = base::GetNormalFilesPathRecurWithOutSort(dir, &files_path);
  assert(ret == base::kOk);

  std::vector<std::string>::iterator vec_it;
  for (vec_it = files_path.begin(); vec_it != files_path.end(); ++vec_it) {
    ret = LogContent(*vec_it, cnt_key, log_interval_logs);
    if (ret != base::kOk) return ret;
  }

  return ret;
} /*}}}*/
}  // namespace tools

#ifdef _LOG_CHECK_MAIN_TEST_
int main(int argc, char *argv[]) {
  using namespace tools;

  fprintf(stderr, "test:%s\n\"", "aa");
  fprintf(stderr, "test:%s\n\\", "aa");
  fprintf(stderr, "test:%s\n\\\"", "aa");
  fprintf(stderr, "test:%s\n\\\\", "aa");
  fprintf(stderr, "test:%s\n\d", "aa");
  fprintf(stderr, "test:%s\n", "aa, bb");
  fprintf(stderr, "test:%s\n", "aa,\"");
  fprintf(stderr, "test:%s\n", "aa,\\");
  fprintf(stderr, "test:%s\n", "aa,\\\"");
  fprintf(stderr, "test:%s\n", "aa,\\\\");
  fprintf(stderr, "test:%s\n", "aa,\a bb");
  fprintf(stderr, "test:%s\n", "())(, aa bb");
  fprintf(stderr, "Invalid palce that should not be arrived!%%27%%%%44%%%%3d\n");

  std::string path = "../../data_process/src/data_process.cc";
  std::string log_name = "LOG";

  CheckLogFormatForCC(path, log_name);
  return 0;
}
#endif
