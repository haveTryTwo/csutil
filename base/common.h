// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMMON_H_
#define BASE_COMMON_H_

#include <string>

namespace base
{

// Maximum length of integer as the string
const int kMaxLenOfInt      =   10;

// Maximun length of ip as the string 
const int kMaxLenOfIp       =   16;

// Unit conversion of time, 1 second = 10^6 microseconds
const int kUnitConvOfMicrosconds        = 1000000;
const int kMillion                      = 1000000;
const int kThousand                     = 1000;

// Default size that struct poll array or struct epoll array
// that would be set
const int kDefaultSizeOfFds             = 10000;

// Default time that the poll or epoll would wait for
const int kDefaultWaitTimeMs            = 100;

const int kHeadLen                  = 4;

const int kBufLen                   = 4096;

const int kDefaultWorkersNum        = 10;

const int kDefaultBacklog           = 1000;

const int kDefaultDelim             = ' ';
const int kNewLine                  = '\n';
const int kDefaulCommitChar         = '#';
const std::string kEqualStr         = "=";

const std::string kDaemonKey        = "daemon";
const std::string kLogPathKey       = "log_path";
const std::string kLogLevelKey      = "log_level";
const std::string kPortKey          = "port";
const std::string kThreadsNumKey    = "threads_num";
const std::string kFlowRestrictKey  = "flow_restrict";
const std::string kStatPathKey      = "stat_path";
const std::string kStatFileSizeKey  = "stat_file_size";
const std::string kStatDumpCirclekey= "stat_dump_circle";

const int kDefaultPort              = 9090;

// statistic info
const int kTimeOutLevelNumber       = 3;
const int kTimeOutLevelOne          = 10;   // milliseconds as time uint
const int kTimeOutLevelTwo          = 100;  // milliseconds as time uint
const int kTimeOutLevelThree        = 500;  // milliseconds as time uint
const int kDefaultStatFileSize      = 32*1024*1024;
const std::string kBackupSuffix     = ".tmp";
const int kDefaultStatDumpCircle    = 60;   // seconds as time unit
const std::string kDefaultStatPath  = "../log/csutil_path";
const std::string kModel            = "csutil";


const int kMaxFlowRestrict          = 1000;
const int kDefaultFlowGridNum       = 1000;
const int kDefaultFlowUnitNum       = 10;

}

#endif
