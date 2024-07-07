// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"

#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "base/mutex.h"
#include "base/status.h"

namespace base {

static const char *GetLogLevel(int level);
static Code CheckLog(const struct tm *cur_tm);

static char g_log_path[kFilePathLen] = {0};
static char g_log_path_fmt[kFilePathLen] = {0};
static int g_log_level = kErrorLevel;
static FILE *g_log_fp = NULL;
// static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
static Mutex g_log_mutex;

static const char *kTooLongMsg = "File name or content too long!";

void SetLogLevel(int level) { /*{{{*/
  if (level >= kDebugLevel || level <= kFatalErrorLevel) {
    g_log_level = level;
  }
} /*}}}*/

void SetLogFmt(const char *log_path_fmt) { /*{{{*/
  if (log_path_fmt != NULL && log_path_fmt[0] != '\0') {
    snprintf(g_log_path_fmt, sizeof(g_log_path_fmt), "%s", log_path_fmt);
  }
} /*}}}*/

void InitLog(const char *log_path_fmt, int level) { /*{{{*/
  SetLogFmt(log_path_fmt);
  SetLogLevel(level);
} /*}}}*/

void CloseLogStream() { /*{{{*/
  if (g_log_fp != NULL) {
    fclose(g_log_fp);
    g_log_fp = NULL;
  }
} /*}}}*/

void DestroyLog() { /*{{{*/
  CloseLogStream();
  //  pthread_mutex_destroy(&g_log_mutex);
} /*}}}*/

void PrintLog(const char *file_path, const char *func, int line_no, int log_level, const char *format, ...) { /*{{{*/
  if (log_level < g_log_level || NULL == file_path || NULL == format) return;

  int ret = kOk;
  time_t cur;
  struct tm cur_tm = {0};
  const char *file_name = NULL;
  char log_buf[kMaxBufLen] = {0};

  file_name = strrchr(file_path, '/');
  file_name = (file_name == NULL) ? file_path : (file_name + 1);

  MutexLock mutex_lock(&g_log_mutex);
  //  pthread_mutex_lock(&g_log_mutex);

  time(&cur);
  localtime_r(&cur, &cur_tm);

  ret = CheckLog(&cur_tm);
  if (kOk == ret) {
    int n = snprintf(log_buf, sizeof(log_buf) - 2, "%4d/%02d/%02d %02d:%02d:%02d - [pid: %d][%s][%s %s %d] - ",
                     cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min,
                     cur_tm.tm_sec, getpid(), GetLogLevel(log_level), file_name, func, line_no);
    if (n >= (sizeof(log_buf) - 2)) {
      fwrite(log_buf, sizeof(char), sizeof(log_buf) - 2, g_log_fp);
      std::string err_msg("; Error message:");
      err_msg.append(kTooLongMsg);
      err_msg.append("\n");
      fwrite(err_msg.c_str(), sizeof(char), err_msg.size(), g_log_fp);
      fflush(g_log_fp);
      return;
    }

    va_list arg_ptr;
    va_start(arg_ptr, format);
    int real_len = vsnprintf(log_buf + n, sizeof(log_buf) - n - 2, format, arg_ptr);
    va_end(arg_ptr);
    if (real_len >= (sizeof(log_buf) - n - 2)) {
      char *extend_buf = new char[real_len + 2];
      if (extend_buf == NULL) {  // NOTE:htt, just log content
        strcat(log_buf, "\n");
        fwrite(log_buf, sizeof(char), sizeof(log_buf) - 1, g_log_fp);
        fflush(g_log_fp);
        return;
      }

      va_list arg_ptr;
      va_start(arg_ptr, format);
      vsnprintf(extend_buf, real_len, format, arg_ptr);
      va_end(arg_ptr);

      fwrite(log_buf, sizeof(char), n, g_log_fp);
      strcat(extend_buf, "\n");
      fwrite(extend_buf, sizeof(char), real_len + 1, g_log_fp);
      fflush(g_log_fp);

      delete[] extend_buf;
      return;
    }

    strcat(log_buf, "\n");
    fwrite(log_buf, sizeof(char), n + real_len + 1, g_log_fp);
    fflush(g_log_fp);
  }

  //  pthread_mutex_unlock(&g_log_mutex);
} /*}}}*/

static Code CheckLog(const struct tm *cur_tm) { /*{{{*/
  Code ret = kOk;
  size_t len = 0;
  char log_path[kFilePathLen] = {0};

  if ('\0' == g_log_path_fmt[0]) {
    g_log_fp = g_log_fp == NULL ? stderr : g_log_fp;
    return kOk;
  }

  len = strftime(log_path, sizeof(log_path), g_log_path_fmt, cur_tm);

  if (strncmp(log_path, g_log_path, len) != 0) {
    if (g_log_fp != NULL && g_log_fp != stderr) {
      fclose(g_log_fp);
      g_log_fp = NULL;
    }

    snprintf(g_log_path, sizeof(g_log_path), "%s", log_path);
    g_log_fp = fopen(g_log_path, "a");
    ret = (g_log_fp == NULL) ? kOpenError : ret;
  } else {
    if (NULL == g_log_fp) {
      g_log_fp = fopen(g_log_path, "a");
      ret = (g_log_fp == NULL) ? kOpenError : ret;
    }
  }

  return ret;
} /*}}}*/

static const char *GetLogLevel(int level) { /*{{{*/
  switch (level) {
    case kFatalErrorLevel:
      return "FATAL ERROR";
    case kErrorLevel:
      return "ERROR";
    case kWarnLevel:
      return "WARNING";
    case kInfoLevel:
      return "INFO";
    case kTraceLevel:
      return "TRACE";
    case kDebugLevel:
      return "DEBUG";
    default:
      return "UNKNOWN";
  }
} /*}}}*/

}  // namespace base

#ifdef _LOG_MAIN_TEST_
int main(int argc, char *argv[]) { /*{{{*/
  using namespace base;
  LOG_FATAL_ERR("fatal err now");
  LOG_ERR("err now");
  LOG_INFO("info now");

  SetLogLevel(kInfoLevel);
  LOG_INFO("info now");

  SetLogFmt("./a.log");
  LOG_FATAL_ERR("fatal err now");
  LOG_ERR("err now");
  LOG_WARN("warning now");
  LOG_INFO("info now");
  LOG_DEBUG("warning now");

  InitLog("./b.log.%Y-%m-%d-%H", kDebugLevel);
  LOG_FATAL_ERR("fatal err now");
  LOG_ERR("err now");
  LOG_WARN("warning now");
  LOG_INFO("info now");
  LOG_TRACE("trace now");
  LOG_DEBUG("debug now");

  DestroyLog();

  return 0;
} /*}}}*/
#endif
