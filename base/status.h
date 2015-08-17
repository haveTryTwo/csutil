// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STATUS_H_ 
#define BASE_STATUS_H_

namespace base
{

enum Code
{/*{{{*/
    kOk             = 0,
    kInvalidParam   = 2,

    kConnError      = 11,
    kSocketError,
    kBindError,
    kListenError,
    kReadError,
    kOpenError,
    kWriteError,
    kAcceptError,
    kConnectError,

    kNotFound       = 51,
    kTimeOut,
    kIOError,
    kFull,
    kPollError,
    kIOCtlError,
    kGetIfsError,

    kCPUError       = 91,

    kPipeFailed     = 131,
    kFcntlFailed,
    kSetsockoptFailed,
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
