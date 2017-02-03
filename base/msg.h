// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MSG_H_
#define BASE_MSG_H_

#include <string>

#include "base/status.h"

namespace base
{

/**
 * Message is the data that will be tranlated in the network;
 * The protocol of message is:
 * [ data length ][data]
 * |-> 4 bytes <-|
 */
Code EncodeToMsg(const std::string &data, std::string *msg);
Code DecodeFromMsg(const std::string &msg, std::string *data);
void PrintBinMsg(const std::string &str);

}

#endif
