// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMMON_H_
#define BASE_COMMON_H_

#include <string>

#include <stdint.h>

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
const int kHundred                      = 100;
const int kTen                          = 10;
const int kFour                         = 4;
const int kSixteen                      = 16;

const int kKB                           = 1024;                 // KiloByte
const int kMB                           = 1024 * 1024;          // MegaByte
const int kGB                           = 1024 * 1024 * 1024;   // GigaByte

// Default size that struct poll array or struct epoll array
// that would be set
const int kDefaultSizeOfFds             = 10000;

// Default time that the poll or epoll would wait for
const int kDefaultWaitTimeMs            = 100;

const int kHeadLen                  = 4;

const int kBufLen                   = 4096;
const int kMaxStreamBufLen          = 16*1024*1024;

const int kDefaultWorkersNum        = 10;

const int kDefaultBacklog           = 1000;

const int kDefaultDelim             = ' ';
const int kWhiteDelim               = ' ';
const int kNewLine                  = '\n';
const int kDefaultCommitChar        = '#';
const int kDot                      = '.';
const int kPercent                  = '%';
const std::string kEqualStr         = "=";
const std::string kUnderLine        = "/";

const std::string kDaemonKey        = "daemon";
const std::string kLogDirKey        = "log_dir";
const std::string kLogFileNameKey   = "log_file_name";
const std::string kLogLevelKey      = "log_level";
const std::string kPortKey          = "port";
const std::string kThreadsNumKey    = "threads_num";
const std::string kFlowRestrictKey  = "flow_restrict";
const std::string kStatPathKey      = "stat_path";
const std::string kStatFileSizeKey  = "stat_file_size";
const std::string kStatDumpCirclekey= "stat_dump_circle";
const std::string kFtpStoreDirKey   = "directory";
const std::string kBufLenKey        = "buf_len";

const int kDefaultPort              = 9090;

// statistic info
const int kTimeOutLevelNumber           = 3;
const int kTimeOutLevelOne              = 10;   // milliseconds as time uint
const int kTimeOutLevelTwo              = 100;  // milliseconds as time uint
const int kTimeOutLevelThree            = 500;  // milliseconds as time uint
const int kDefaultStatFileSize          = 32*1024*1024;
const std::string kBackupSuffix         = ".tmp";
const int kDefaultStatDumpCircle        = 60;   // seconds as time unit
const std::string kDefaultStatPath      = "../log/csutil_path";
const std::string kModel                = "csutil";
const std::string kDefaultFtpStoreDir   = "../www";
const int kDefaultBufLen                = 4096;
const int kSmallBufLen                  = 64;


const int kMaxFlowRestrict          = 1000;
const int kDefaultFlowGridNum       = 1000;
const int kDefaultFlowUnitNum       = 10;

const int kDefaultDirMode           = 0755;
const int kDefaultRegularFileMode   = 0644;

const std::string kInvalidCntSuffix     = "invalid_cnt";
const std::string kSortStatSuffix       = "sort_stat";
const char kStatDelim                   = ':';

const std::string kJan              = "Jan";
const std::string kFeb              = "Feb";
const std::string kMar              = "Mar";
const std::string kApr              = "Apr";
const std::string kMay              = "May";
const std::string kJun              = "Jun";
const std::string kJul              = "Jul";
const std::string kAug              = "Aug";
const std::string kSep              = "Sep";
const std::string kOct              = "Oct";
const std::string kNov              = "Nov";
const std::string kDec              = "Dec";

const uint32_t kCoroutineDefaultId  = 0;
const uint32_t kCoroutineStartId    = 1;
const uint32_t kMaxCoroutines       = 1024*1024*1024;

enum CoroutineStatus 
{
    kCoroutineNotExistStatus     = 0,
    kCoroutinePreparingStatus   = 1,
    kCoroutineRunningStatus     = 2,
    kCoroutineSuspendingStatus  = 3,
};

const uint32_t kConsistentHashSeed = 0x0523;

}

#endif
