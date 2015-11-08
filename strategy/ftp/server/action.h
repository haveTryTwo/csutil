// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FTP_SERVER_ACTION_H_
#define FTP_SERVER_ACTION_H_

#include <string>

#include "base/status.h"
#include "base/config.h"

namespace ftp
{

base::Code FtpAction(const base::Config &conf, const std::string &in, std::string *out);

}

#endif
