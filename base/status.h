// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STATUS_H_ 
#define BASE_STATUS_H_

#include <string>

namespace base
{

const std::string FlowInfo = "Now flow restrict!";
const std::string ActionFailedInfo = "Failed to do user action";


enum Code
{/*{{{*/
    kOtherFailed            = 65535,

    kExitOk                 = 1,
    kOk                     = 0,
    kDataNotEnough          = 1,    // data read from tcp stream is not enough

    kInvalidParam           = 2,
    kInvalidPlace           = 3,    // Invalid place that should not be arrived

    kConnError              = 11,
    kSocketError            = 12,
    kBindError              = 13,
    kListenError            = 14,
    kReadError              = 15,
    kOpenError              = 16,
    kWriteError             = 17,
    kAcceptError            = 18,
    kConnectError           = 19,
    kDataBufFull            = 20,

    kNotFound               = 51,
    kTimeOut                = 52,
    kIOError                = 53,
    kFull                   = 54,
    kPollError              = 55,
    kIOCtlError             = 56,
    kGetIfsError            = 57,
    kForkFailed             = 58,
    kStatFailed             = 59,
    kRenameFailed           = 60,
    kEpollCreateFailed      = 61,
    kEpollFailed            = 62,
    kEpollCtlFailed         = 63,
    kIpOrPortNotInit        = 64,

    kCPUError               = 91,

    kPipeFailed             = 131,
    kFcntlFailed            = 132,
    kSetsockoptFailed       = 133,

    kPthreadCreateFailed    = 151,

    kStrtollFailed          = 181,
    kNoDigits               = 182,
    kNotAllDigits           = 183,

    kFlowRestrict           = 201,

    kDlopenFailed           = 211,
    kDlsymFailed            = 212,

    kMkdirFailed            = 231,

    kInvalidPbMessage       = 1001,
    kInvalidMessageType     = 1002,

    kFileNotExist           = 5001,
    kFileIsEnd              = 5002,
    kInvaliedFileStartPos   = 5003,
    kFileNameNotSet         = 5004,
    kOpenFileFailed         = 5005,
    kNotDir                 = 5006,
    kOpenDirFailed          = 5007,
    kFgetsFailed            = 5008,

    kMkTimeFailed           = 5100,
    kLocalTimeFailed        = 5101,

    kCurlGlobalInitFailed   = 5201,
    kCurlEasyInitFailed     = 5202,
    kCurlEasyPerformFailed  = 5203,

    kInvalidHttpType        = 5301,
    kHttpRedirect           = 5302,
    kInvalidHttpRetStatus   = 5303,
    kHttpNoCRLFCRLF         = 5304,
    kInvalidHttpResp        = 5305,
    kGetHostByNameFailed    = 5306,
    kNetAddrConvertFailed   = 5307,

    kRegCompFailed          = 5401,
    kRegNotMatch            = 5402,

    kGetRlimitFailed        = 5501,
    kSetRlimitFailed        = 5502,

    kInvalidUrlEncodingStr  = 5601,

    kInvalidWayOfReadingFile    = 5701,
    kInvalidWayOfWritingFile    = 5702,
    kCheckFileFailed            = 5703,

    kCASFailed                  = 5801,

    kInvalidPath                = 5901,
    kNodeExist                  = 5902,

    kExist                      = 6001,
};/*}}}*/

class Status
{/*{{{*/
    public:
        explicit Status(Code c) : code_(c) {}

        Status(const Status &status)
        {
            code_ = status.code_;
        }

        Status& operator= (const Status &status)
        {
            code_ = status.code_;
            return *this;
        }

        ~Status() {}

    private:
        Code code_;
};/*}}}*/

}

#endif
