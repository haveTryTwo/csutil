// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elevator.h"

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <vector>

namespace base {

static uint32_t AbsDiff(uint32_t a, uint32_t b) { /*{{{*/
  return (a > b) ? (a - b) : (b - a);
} /*}}}*/

Code ElevatorFCFS(uint32_t current_floor, const std::vector<uint32_t>& requests,
                  ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  uint32_t pos = current_floor;
  for (uint32_t i = 0; i < requests.size(); ++i) {
    result->total_distance += AbsDiff(pos, requests[i]);
    pos = requests[i];
    result->serve_order.push_back(requests[i]);
  }

  return kOk;
} /*}}}*/

Code ElevatorSSTF(uint32_t current_floor, const std::vector<uint32_t>& requests,
                  ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  std::vector<bool> visited(requests.size(), false);
  uint32_t pos = current_floor;

  for (uint32_t served = 0; served < requests.size(); ++served) {
    uint32_t min_dist = UINT32_MAX;
    uint32_t nearest_idx = 0;
    for (uint32_t i = 0; i < requests.size(); ++i) {
      if (visited[i]) continue;
      uint32_t dist = AbsDiff(pos, requests[i]);
      if (dist < min_dist) {
        min_dist = dist;
        nearest_idx = i;
      }
    }
    visited[nearest_idx] = true;
    result->total_distance += min_dist;
    pos = requests[nearest_idx];
    result->serve_order.push_back(requests[nearest_idx]);
  }

  return kOk;
} /*}}}*/

Code ElevatorSCAN(uint32_t current_floor, ElevatorDirection direction,
                  const std::vector<uint32_t>& requests, uint32_t max_floor,
                  ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;
  if (current_floor > max_floor) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  std::vector<uint32_t> sorted_req(requests);
  std::sort(sorted_req.begin(), sorted_req.end());

  std::vector<uint32_t> lower;
  std::vector<uint32_t> upper;
  for (uint32_t i = 0; i < sorted_req.size(); ++i) {
    if (sorted_req[i] < current_floor) {
      lower.push_back(sorted_req[i]);
    } else {
      upper.push_back(sorted_req[i]);
    }
  }

  uint32_t pos = current_floor;
  if (direction == kElevatorUp) {
    for (uint32_t i = 0; i < upper.size(); ++i) {
      result->serve_order.push_back(upper[i]);
    }
    if (!lower.empty()) {
      result->serve_order.push_back(max_floor);
      for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
        result->serve_order.push_back(lower[i]);
      }
    }
  } else {
    for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
      result->serve_order.push_back(lower[i]);
    }
    if (!upper.empty()) {
      result->serve_order.push_back(0);
      for (uint32_t i = 0; i < upper.size(); ++i) {
        result->serve_order.push_back(upper[i]);
      }
    }
  }

  for (uint32_t i = 0; i < result->serve_order.size(); ++i) {
    result->total_distance += AbsDiff(pos, result->serve_order[i]);
    pos = result->serve_order[i];
  }

  // 移除辅助端点（max_floor 或 0），它们不是真正的请求
  std::vector<uint32_t> filtered;
  for (uint32_t i = 0; i < result->serve_order.size(); ++i) {
    bool is_real_request = false;
    for (uint32_t j = 0; j < requests.size(); ++j) {
      if (result->serve_order[i] == requests[j]) {
        is_real_request = true;
        break;
      }
    }
    if (is_real_request) {
      filtered.push_back(result->serve_order[i]);
    }
  }
  result->serve_order = filtered;

  return kOk;
} /*}}}*/

Code ElevatorLOOK(uint32_t current_floor, ElevatorDirection direction,
                  const std::vector<uint32_t>& requests,
                  ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  std::vector<uint32_t> sorted_req(requests);
  std::sort(sorted_req.begin(), sorted_req.end());

  std::vector<uint32_t> lower;
  std::vector<uint32_t> upper;
  for (uint32_t i = 0; i < sorted_req.size(); ++i) {
    if (sorted_req[i] < current_floor) {
      lower.push_back(sorted_req[i]);
    } else {
      upper.push_back(sorted_req[i]);
    }
  }

  uint32_t pos = current_floor;
  if (direction == kElevatorUp) {
    for (uint32_t i = 0; i < upper.size(); ++i) {
      result->serve_order.push_back(upper[i]);
    }
    for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
      result->serve_order.push_back(lower[i]);
    }
  } else {
    for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
      result->serve_order.push_back(lower[i]);
    }
    for (uint32_t i = 0; i < upper.size(); ++i) {
      result->serve_order.push_back(upper[i]);
    }
  }

  for (uint32_t i = 0; i < result->serve_order.size(); ++i) {
    result->total_distance += AbsDiff(pos, result->serve_order[i]);
    pos = result->serve_order[i];
  }

  return kOk;
} /*}}}*/

Code ElevatorCSCAN(uint32_t current_floor, ElevatorDirection direction,
                   const std::vector<uint32_t>& requests, uint32_t max_floor,
                   ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;
  if (current_floor > max_floor) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  std::vector<uint32_t> sorted_req(requests);
  std::sort(sorted_req.begin(), sorted_req.end());

  std::vector<uint32_t> lower;
  std::vector<uint32_t> upper;
  for (uint32_t i = 0; i < sorted_req.size(); ++i) {
    if (sorted_req[i] < current_floor) {
      lower.push_back(sorted_req[i]);
    } else {
      upper.push_back(sorted_req[i]);
    }
  }

  uint32_t pos = current_floor;
  if (direction == kElevatorUp) {
    // 先向上服务，到 max_floor，再从 0 开始向上服务剩余
    for (uint32_t i = 0; i < upper.size(); ++i) {
      result->total_distance += AbsDiff(pos, upper[i]);
      pos = upper[i];
      result->serve_order.push_back(upper[i]);
    }
    if (!lower.empty()) {
      result->total_distance += AbsDiff(pos, max_floor);
      // 从 max_floor 跳到 0（跳转不计距离）
      pos = 0;
      for (uint32_t i = 0; i < lower.size(); ++i) {
        result->total_distance += AbsDiff(pos, lower[i]);
        pos = lower[i];
        result->serve_order.push_back(lower[i]);
      }
    }
  } else {
    // 先向下服务，到 0，再从 max_floor 开始向下服务剩余
    for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
      result->total_distance += AbsDiff(pos, lower[i]);
      pos = lower[i];
      result->serve_order.push_back(lower[i]);
    }
    if (!upper.empty()) {
      result->total_distance += AbsDiff(pos, 0u);
      // 从 0 跳到 max_floor（跳转不计距离）
      pos = max_floor;
      for (int32_t i = static_cast<int32_t>(upper.size()) - 1; i >= 0; --i) {
        result->total_distance += AbsDiff(pos, upper[i]);
        pos = upper[i];
        result->serve_order.push_back(upper[i]);
      }
    }
  }

  return kOk;
} /*}}}*/

Code ElevatorCLOOK(uint32_t current_floor, ElevatorDirection direction,
                   const std::vector<uint32_t>& requests,
                   ElevatorResult* result) { /*{{{*/
  if (result == NULL) return kInvalidParam;

  result->serve_order.clear();
  result->total_distance = 0;

  if (requests.empty()) return kOk;

  std::vector<uint32_t> sorted_req(requests);
  std::sort(sorted_req.begin(), sorted_req.end());

  std::vector<uint32_t> lower;
  std::vector<uint32_t> upper;
  for (uint32_t i = 0; i < sorted_req.size(); ++i) {
    if (sorted_req[i] < current_floor) {
      lower.push_back(sorted_req[i]);
    } else {
      upper.push_back(sorted_req[i]);
    }
  }

  uint32_t pos = current_floor;
  if (direction == kElevatorUp) {
    // 先向上服务，到最后一个请求，再从最低请求开始向上服务
    for (uint32_t i = 0; i < upper.size(); ++i) {
      result->total_distance += AbsDiff(pos, upper[i]);
      pos = upper[i];
      result->serve_order.push_back(upper[i]);
    }
    if (!lower.empty()) {
      // 跳到最低请求（跳转不计距离）
      pos = lower[0];
      for (uint32_t i = 0; i < lower.size(); ++i) {
        result->total_distance += AbsDiff(pos, lower[i]);
        pos = lower[i];
        result->serve_order.push_back(lower[i]);
      }
    }
  } else {
    // 先向下服务，到最后一个请求，再从最高请求开始向下服务
    for (int32_t i = static_cast<int32_t>(lower.size()) - 1; i >= 0; --i) {
      result->total_distance += AbsDiff(pos, lower[i]);
      pos = lower[i];
      result->serve_order.push_back(lower[i]);
    }
    if (!upper.empty()) {
      // 跳到最高请求（跳转不计距离）
      pos = upper[upper.size() - 1];
      for (int32_t i = static_cast<int32_t>(upper.size()) - 1; i >= 0; --i) {
        result->total_distance += AbsDiff(pos, upper[i]);
        pos = upper[i];
        result->serve_order.push_back(upper[i]);
      }
    }
  }

  return kOk;
} /*}}}*/

}  // namespace base
