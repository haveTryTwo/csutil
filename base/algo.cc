// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/algo.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <utility>
#include <vector>

namespace base {

Code LCS(const std::string &seq_first, const std::string &seq_second, std::string *lcs_str) { /*{{{*/
  if (lcs_str == NULL) return kInvalidParam;

  uint32_t row_num = seq_first.size() + 1;
  uint32_t col_num = seq_second.size() + 1;
  int **tmp_arr = new int *[row_num];
  if (tmp_arr == NULL) return kMallocFailed;
  for (uint32_t i = 0; i < row_num; ++i) {
    tmp_arr[i] = new int[col_num];
    memset(tmp_arr[i], '\0', col_num * sizeof(int));
  }

  for (uint32_t i = 1; i < row_num; ++i) {
    for (uint32_t j = 1; j < col_num; ++j) {
      if (seq_first.data()[i - 1] == seq_second.data()[j - 1]) {
        tmp_arr[i][j] = tmp_arr[i - 1][j - 1] + 1;
      } else {
        tmp_arr[i][j] = tmp_arr[i - 1][j] > tmp_arr[i][j - 1] ? tmp_arr[i - 1][j] : tmp_arr[i][j - 1];
      }
    }
  }

  std::string tmp_str;
  for (uint32_t i = row_num - 1, j = col_num - 1; i >= 1 && j >= 1;) {
    if (seq_first.data()[i - 1] == seq_second.data()[j - 1]) {
      tmp_str.append(1, seq_first.data()[i - 1]);
      --i;
      --j;
    } else {
      if (tmp_arr[i - 1][j] > tmp_arr[i][j - 1])
        i--;
      else
        j--;
    }
  }

  for (uint32_t i = 0; i < tmp_str.size(); ++i) {
    lcs_str->append(1, tmp_str.data()[tmp_str.size() - i - 1]);
  }

  for (uint32_t i = 0; i < row_num; ++i) {
    delete[] tmp_arr[i];
  }

  delete[] tmp_arr;

  return kOk;
} /*}}}*/

Code LCSS(const std::string &seq_first, const std::string &seq_second, std::string *lcss_str) { /*{{{*/
  if (lcss_str == NULL) return kInvalidParam;

  uint32_t row_num = seq_first.size() + 1;
  uint32_t col_num = seq_second.size() + 1;
  int max_lcss_len = 0;
  uint32_t max_lcss_row = 0;
  // uint32_t max_lcss_col = 0;

  int **tmp_arr = new int *[row_num];
  if (tmp_arr == NULL) return kMallocFailed;
  for (uint32_t i = 0; i < row_num; ++i) {
    tmp_arr[i] = new int[col_num];
    memset(tmp_arr[i], '\0', col_num * sizeof(int));
  }

  for (uint32_t i = 1; i < row_num; ++i) { /*{{{*/
    for (uint32_t j = 1; j < col_num; ++j) {
      if (seq_first.data()[i - 1] == seq_second.data()[j - 1]) {
        tmp_arr[i][j] = tmp_arr[i - 1][j - 1] + 1;

        if (max_lcss_len < tmp_arr[i][j]) {
          max_lcss_len = tmp_arr[i][j];
          max_lcss_row = i;
          // max_lcss_col = j;
        }
      } else {
        tmp_arr[i][j] = 0;
      }
    }
  } /*}}}*/

  lcss_str->assign(seq_first.data(), max_lcss_row - max_lcss_len, max_lcss_len);

  for (uint32_t i = 0; i < row_num; ++i) {
    delete[] tmp_arr[i];
  }

  delete[] tmp_arr;

  return kOk;
} /*}}}*/

Code IsPrime(uint64_t num, bool *is_prime) { /*{{{*/
  if (is_prime == NULL) return kInvalidParam;
  if (num == 0 || num == 1) {
    *is_prime = false;
    return kOk;
  }

  uint64_t square = sqrt(num);
  *is_prime = true;
  for (uint64_t i = 2; i <= square; ++i) {
    if (num % i == 0) {
      *is_prime = false;
      break;
    }
  }

  return kOk;
} /*}}}*/

// Calculate Euclidean distance
Code CalculteDistance(const std::vector<double> &first_point, const std::vector<double> &second_point,
                      double *distance) { /*{{{*/
  if (first_point.size() != second_point.size() || distance == NULL) return kInvalidParam;

  double tmp_dist = 0;
  for (size_t i = 0; i < first_point.size(); ++i) {
    tmp_dist += (first_point[i] - second_point[i]) * (first_point[i] - second_point[i]);
  }

  *distance = sqrt(tmp_dist);
  return kOk;
} /*}}}*/

// kNN function
Code kNN(const std::vector<std::vector<double>> &points, const std::vector<double> query_point, int k,
         std::vector<std::vector<double>> *neighbors) { /*{{{*/
  if (k < 0 || neighbors == NULL) return kInvalidParam;
  neighbors->clear();

  Code ret = kOk;
  std::vector<std::pair<double, std::vector<double>>> distances;
  for (const auto &point : points) {
    double dist = 0;
    ret = CalculteDistance(query_point, point, &dist);
    if (ret != kOk) return ret;
    distances.push_back(std::make_pair(dist, point));
  }

  std::sort(distances.begin(), distances.end());
  for (int i = 0; i < k; ++i) {
    neighbors->push_back(distances[i].second);
  }

  return ret;
} /*}}}*/

}  // namespace base
