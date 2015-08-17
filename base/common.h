// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_COMMON_H_
#define BASE_COMMON_H_

namespace base
{

// Maximum length of integer as the string
const int kMaxLenOfInt      =   10;

// Maximun length of ip as the string 
const int kMaxLenOfIp       =   16;

// Unit conversion of time, 1 second = 10^6 microseconds
const int kUnitConvOfMicrosconds        = 1000000;

// Default size that struct poll array or struct epoll array
// that would be set
const int kDefaultSizeOfFds             = 10000;

// Default time that the poll or epoll would wait for
const int kDefaultWaitTimeMs            = 100;

const int kHeadLen          = 4;

const int kBufLen           = 4096;

const int kDefaultWorkersNum    = 10;

const int kDefaultBacklog       = 1000;

}

#endif
