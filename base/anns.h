// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANNS_H_
#define BASE_ANNS_H_

#include <vector>

#include "base/common.h"
#include "base/status.h"

namespace base {

// Brute force nearest neighbor search
Code BruteForceANNS(const std::vector<std::vector<double>>& data, const std::vector<double>& query,
                    std::vector<double>* nearest);

}  // namespace base
#endif
