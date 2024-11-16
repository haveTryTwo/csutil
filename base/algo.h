// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALGO_H_
#define BASE_ALGO_H_

#include <stdint.h>

#include <vector>

#include "base/status.h"

namespace base {

// Note: Longest common sequence
Code LCS(const std::string &seq_first, const std::string &seq_second, std::string *lcs_str);

// Note: Longest common substring
Code LCSS(const std::string &seq_first, const std::string &seq_second, std::string *lcss_str);

// NOTE:htt, 验证是否为质数
Code IsPrime(uint64_t num, bool *is_prime);

Code CalculteDistance(const std::vector<double> &first_point, const std::vector<double> &second_point,
                      double *distance);

// kNN function: https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm
Code kNN(const std::vector<std::vector<double>> &points, const std::vector<double> query_point, int k,
         std::vector<std::vector<double>> *neighbors);

}  // namespace base

#endif
