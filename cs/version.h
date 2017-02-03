// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CS_VERSION_H_
#define CS_VERSION_H_

namespace base
{

const std::string kCSVerson("1.0.0");

inline std::string GetVersion()
{
    return kCSVerson;
}

}

#endif
