// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_STATUS_H_
#define BASE_STATUS_H_

#include <string>

namespace base {

const std::string FlowInfo = "Now flow restrict!";
const std::string ActionFailedInfo = "Failed to do user action";

enum Code { /*{{{*/
            kOtherFailed = 65535,

            kExitOk = 1,
            kOk = 0,
            kDataNotEnough = 1,  // data read from tcp stream is not enough

            kInvalidParam = 2,
            kInvalidPlace = 3,  // Invalid place that should not be arrived
            kNotInit = 4,       // Class hasn't been initialized
            kNotEqual = 5,
            kLess = 6,

            kConnError = 11,
            kSocketError = 12,
            kBindError = 13,
            kListenError = 14,
            kReadError = 15,
            kOpenError = 16,
            kWriteError = 17,
            kAcceptError = 18,
            kConnectError = 19,
            kDataBufFull = 20,

            kNotFound = 51,
            kTimeOut = 52,
            kIOError = 53,
            kFull = 54,
            kPollError = 55,
            kIOCtlError = 56,
            kGetIfsError = 57,
            kForkFailed = 58,
            kStatFailed = 59,
            kRenameFailed = 60,
            kEpollCreateFailed = 61,
            kEpollFailed = 62,
            kEpollCtlFailed = 63,
            kIpOrPortNotInit = 64,
            kInvalidIp = 65,
            kStatfsFailed = 66,

            kCPUError = 91,

            kPipeFailed = 131,
            kFcntlFailed = 132,
            kSetsockoptFailed = 133,

            kPthreadCreateFailed = 151,

            kStrtollFailed = 181,
            kNoDigits = 182,
            kNotAllDigits = 183,
            kNotPostiveDigits = 184,
            kSubError = 185,

            kFlowRestrict = 201,

            kDlopenFailed = 211,
            kDlsymFailed = 212,

            kMkdirFailed = 231,

            kInvalidPbMessage = 1001,
            kInvalidMessageType = 1002,

            kFileNotExist = 5001,
            kFileIsEnd = 5002,
            kInvaliedFileStartPos = 5003,
            kFileNameNotSet = 5004,
            kOpenFileFailed = 5005,
            kNotDir = 5006,
            kOpenDirFailed = 5007,
            kFgetsFailed = 5008,
            kReadFileLenExtendMax = 5009,

            kMkTimeFailed = 5100,
            kLocalTimeFailed = 5101,

            kCurlGlobalInitFailed = 5201,
            kCurlEasyInitFailed = 5202,
            kCurlEasyPerformFailed = 5203,

            kInvalidHttpType = 5301,
            kHttpRedirect = 5302,
            kInvalidHttpRetStatus = 5303,
            kHttpNoCRLFCRLF = 5304,
            kInvalidHttpResp = 5305,
            kGetHostByNameFailed = 5306,
            kNetAddrConvertFailed = 5307,
            kNoContentLength = 5308,

            kRegCompFailed = 5401,
            kRegNotMatch = 5402,

            kGetRlimitFailed = 5501,
            kSetRlimitFailed = 5502,

            kInvalidUrlEncodingStr = 5601,

            kInvalidWayOfReadingFile = 5701,
            kInvalidWayOfWritingFile = 5702,
            kCheckFileFailed = 5703,

            kCASFailed = 5801,

            kInvalidPath = 5901,
            kNodeExist = 5902,

            kExist = 6001,

            kMallocFailed = 7001,

            kDataValueError = 8001,
            kValueSizeIsLarger = 8002,
            kDBFull = 8003,

            kInvalidLength = 9001,
            kInvalidRegExp = 9002,
            kInvalidRange = 9003,
            kInvalidInBracket = 9004,
            kInvalidMetaOperator = 9005,
            kInvalidRegEnd = 9006,
            kParenNumExceed = 9007,
            kNoNextNode = 9008,
            kInvalidRegNfa = 9009,

            kEncryptFailed = 10001,
            kDecryptFailed = 10002,
            kSHA1Failed = 10003,
            kRSASignFailed = 10004,
            kRSAVerifyFailed = 10005,
            kInvalidSignSize = 10006,
            kNewFailed = 10007,
            kReadFailed = 10008,
            kInvalidEncryptDataSize = 10009,
            kEVPEncryptInitExFailed = 10010,
            kEVPEncryptUpdateFailed = 10011,
            kEVPEncryptFinalExFailed = 10012,
            kEVPDecryptInitExFailed = 10013,
            kEVPDecryptUpdateFailed = 10014,
            kEVPDecryptFinalExFailed = 10015,
            kInvalidAESKeyLenFlag = 10016,

            kCoroutinesTooMach = 10101,
            kInvalidCoroutineId = 10102,
            kFailedGetContext = 10103,
            kInvalidStatus = 10104,
            kFailedSwapContext = 10105,

            kInvalidBitCaskFileName = 10201,
            kFilesNumIsFull = 10202,
            kDataIsNotConsistent = 10203,
            kInvalidFileName = 10204,
            kKeySizeIsLarge = 10205,
            kTimeWrong = 10206,

            kRingEmpty = 10301,

            kNotAllLowerCaseLetter = 10401,
            kNotFullWord = 10402,

            kImportFailed = 10501,

}; /*}}}*/

class Status { /*{{{*/
 public:
  explicit Status(Code c) : code_(c) {}

  Status(const Status &status) { code_ = status.code_; }

  Status &operator=(const Status &status) {
    code_ = status.code_;
    return *this;
  }

  ~Status() {}

 private:
  Code code_;
}; /*}}}*/

}  // namespace base

#endif
