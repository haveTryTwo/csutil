// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOG_H_
#define BASE_LOG_H_

namespace base
{

const int kFilePathLen        = 2048;
const int kMaxBufLen          = 2048;
   
enum LogLevel
{
    kDebugLevel = 0,
    kTraceLevel,
    kInfoLevel, 
    kWarnLevel,
    kErrorLevel,
    kFatalErrorLevel,
};

#define LOG_FATAL_ERR(format, ...) PrintLog(__FILE__, __func__, __LINE__, kFatalErrorLevel, format, ## __VA_ARGS__)
#define LOG_ERR(format, ...) PrintLog(__FILE__, __func__, __LINE__, kErrorLevel, format, ## __VA_ARGS__)
#define LOG_WARN(format, ...) PrintLog(__FILE__, __func__, __LINE__, kWarnLevel, format, ## __VA_ARGS__)
#define LOG_INFO(format, ...) PrintLog(__FILE__, __func__, __LINE__, kInfoLevel, format, ## __VA_ARGS__)
#define LOG_TRACE(format, ...) PrintLog(__FILE__, __func__, __LINE__, kTraceLevel, format, ## __VA_ARGS__)
#define LOG_DEBUG(format, ...) PrintLog(__FILE__, __func__, __LINE__, kDebugLevel, format, ## __VA_ARGS__)

void SetLogLevel(int level);
void SetLogFmt(const char *log_path_fmt);
void InitLog(const char *log_path_fmt, int level);
void CloseLogStream();
void DestroyLog();
void PrintLog(const char *file_path, const char *func, int line_no, int log_level, const char* format, ...);

}

#endif
