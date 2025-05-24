// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BKD_TREE_H_
#define BASE_BKD_TREE_H_

#include <stdint.h>

#include <vector>

#include "base/status.h"

namespace base {

struct BkdTreePoint {
  std::vector<int64_t> coords;
  BkdTreePoint(int64_t k) : coords(k) {}
  BkdTreePoint(std::initializer_list<int64_t> l) : coords(l) {}
};

class BKDTree {
 public:
  struct Node {
    BkdTreePoint point;
    Node* left;
    Node* right;
    Node(const BkdTreePoint& pt) : point(pt), left(nullptr), right(nullptr) {}
  };

  BKDTree(int64_t k) : k_(k), root_(nullptr) {}

  void insert(const BkdTreePoint& pt) { root_ = insertRec(root_, pt, 0); }

  // 范围查询：返回所有在 [low, high] 区间内的点
  void rangeQuery(const BkdTreePoint& low, const BkdTreePoint& high, std::vector<BkdTreePoint>& result) {
    rangeQueryRec(root_, low, high, 0, result);
  }

  ~BKDTree() { destroy(root_); }

 private:
  int k_;
  Node* root_;

  Node* insertRec(Node* node, const BkdTreePoint& pt, int depth) {
    if (!node) return new Node(pt);
    int dim = depth % k_;
    if (pt.coords[dim] < node->point.coords[dim])
      node->left = insertRec(node->left, pt, depth + 1);
    else
      node->right = insertRec(node->right, pt, depth + 1);
    return node;
  }

  void rangeQueryRec(Node* node, const BkdTreePoint& low, const BkdTreePoint& high, int depth,
                     std::vector<BkdTreePoint>& result) {
    if (!node) return;
    bool inside = true;
    for (int i = 0; i < k_; ++i) {
      if (node->point.coords[i] < low.coords[i] || node->point.coords[i] > high.coords[i]) {
        inside = false;
        break;
      }
    }
    if (inside) result.push_back(node->point);

    int dim = depth % k_;
    if (node->left && low.coords[dim] <= node->point.coords[dim])
      rangeQueryRec(node->left, low, high, depth + 1, result);
    if (node->right && high.coords[dim] >= node->point.coords[dim])
      rangeQueryRec(node->right, low, high, depth + 1, result);
  }

  void destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
  }
};

}  // namespace base
#endif  // BASE_BKD_TREE_H_
