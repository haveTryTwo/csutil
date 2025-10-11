// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/anns.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include <cmath>
#include <functional>
#include <limits>
#include <utility>

#include "base/algo.h"

namespace base {

Code BruteForceANNS(const std::vector<std::vector<double>>& data, const std::vector<double>& query,
                    std::vector<double>* nearest) {
  if (nearest == NULL) return kInvalidParam;
  if (data.empty()) return kInvalidParam;

  double min_dist = std::numeric_limits<double>::max();
  size_t best_idx = SIZE_MAX;  // Initialization with invalid value
  bool found_valid = false;

  for (size_t i = 0; i < data.size(); ++i) {
    const auto& point = data[i];
    double dist = 0;
    Code ret = CalculteDistance(point, query, &dist);
    if (ret != kOk) return ret;

    if (dist < min_dist) {
      min_dist = dist;
      best_idx = i;
      found_valid = true;
    }
  }

  // Safety check: ensuring a valid nearest neighbor is found
  if (!found_valid || best_idx >= data.size()) {
    return kInvalidPlace;
  }

  *nearest = data[best_idx];
  return kOk;
}

}  // namespace base
