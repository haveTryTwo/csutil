// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/anns.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <new>
#include <random>
#include <unordered_set>
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

KDTree::KDTree(const std::vector<KDPoint>& point) : points_(point), root_(nullptr) {}

KDTree::~KDTree() {
  DestroyTree(root_);
  root_ = nullptr;
}

// 递归销毁KD树节点
void KDTree::DestroyTree(KDNode* node) {
  if (node == nullptr) return;

  // 递归销毁左右子树
  DestroyTree(node->left);
  DestroyTree(node->right);

  // 清理节点的左右指针，防止悬空指针
  node->left = nullptr;
  node->right = nullptr;

  // 删除当前节点
  delete node;
}

// 构建 KD-Tree
Code KDTree::Init() {
  // 清理之前的树
  DestroyTree(root_);
  root_ = nullptr;
  return InitInternal(points_, &root_, 0);
}

Code KDTree::InitInternal(std::vector<KDPoint>& points, KDNode** build_node, int depth) {
  if (points.empty() || build_node == nullptr) return kInvalidParam;

  // 防止递归深度过深导致栈溢出
  if (depth > 64) return kInvalidParam;  // 限制最大深度

  // 检查维度一致性
  if (points[0].coords.empty()) return kInvalidParam;
  int k = static_cast<int>(points[0].coords.size());
  if (k <= 0) return kInvalidParam;

  // 验证所有点的维度是否一致
  for (const auto& point : points) {
    if (static_cast<int>(point.coords.size()) != k) {
      return kInvalidParam;
    }
  }

  int axis = depth % k;  // 选择划分轴

  // 按当前轴排序
  // 预先检查axis是否有效
  if (axis < 0 || axis >= k) return kInternalError;

  // 检查所有点在指定轴上是否有有效值
  for (const auto& point : points) {
    if (axis >= static_cast<int>(point.coords.size())) {
      return kInvalidParam;
    }
    // 检查是否为有效的浮点数
    double val = point.coords[axis];
    if (std::isnan(val) || std::isinf(val)) {
      return kInvalidParam;
    }
  }

  // 执行排序，std::sort通常不会抛出异常，除非比较函数抛出异常
  // 我们的lambda比较函数只做简单的数值比较，不会抛出异常
  std::sort(points.begin(), points.end(),
            [axis](const KDPoint& a, const KDPoint& b) { return a.coords[axis] < b.coords[axis]; });

  // 找到中位数作为根节点
  size_t median = points.size() / 2;

  // 检查median索引是否有效
  if (median >= points.size()) return kInternalError;

  // 使用nothrow版本的new，避免异常
  KDNode* node = new (std::nothrow) KDNode(points[median]);

  if (node == nullptr) return kNewFailed;

  // 初始化节点指针为nullptr，确保异常安全
  node->left = nullptr;
  node->right = nullptr;
  *build_node = node;

  // 使用局部变量管理子树构建，确保异常安全
  KDNode* left_child = nullptr;
  KDNode* right_child = nullptr;

  // 递归构建左右子树
  if (median > 0) {  // 有左子树
    std::vector<KDPoint> left_points(points.begin(), points.begin() + median);
    Code ret = InitInternal(left_points, &left_child, depth + 1);
    if (ret != kOk) {
      // 左子树构建失败，清理当前节点
      delete node;
      *build_node = nullptr;
      return ret;
    }
  }

  if (median + 1 < points.size()) {  // 有右子树
    std::vector<KDPoint> right_points(points.begin() + median + 1, points.end());
    Code ret = InitInternal(right_points, &right_child, depth + 1);
    if (ret != kOk) {
      // 右子树构建失败，需要清理左子树和当前节点
      if (left_child != nullptr) {
        DestroyTree(left_child);
        left_child = nullptr;
      }
      delete node;
      *build_node = nullptr;
      return ret;
    }
  }

  // 只有在所有子树都成功构建后，才将它们赋值给当前节点
  // 这样可以避免在DestroyTree中重复删除
  node->left = left_child;
  node->right = right_child;

  return kOk;
}

// 最近邻搜索
Code KDTree::NearestNeighbor(const KDPoint& query, KDPoint& best, double& best_dist) {
  if (root_ == nullptr || query.coords.empty()) return kInvalidParam;

  // 初始化最佳距离为无穷大
  best_dist = std::numeric_limits<double>::max();

  return NearestNeighborInternal(root_, query, 0, best, best_dist);
}

Code KDTree::NearestNeighborInternal(const KDNode* node, const KDPoint& query, int depth, KDPoint& best,
                                     double& best_dist) {
  if (node == nullptr) return kOk;  // 空节点不是错误，直接返回

  // 防止递归深度过深
  if (depth > 64) return kInternalError;

  // 验证维度一致性
  if (query.coords.size() != node->point.coords.size()) {
    return kInvalidParam;
  }

  int k = static_cast<int>(query.coords.size());
  if (k <= 0) return kInvalidParam;

  int axis = depth % k;

  // 计算当前节点与查询点的距离
  double dist = 0;
  Code ret = CalculteDistance(node->point.coords, query.coords, &dist);
  if (ret != kOk) return ret;

  // 更新最佳结果
  if (dist < best_dist) {
    best_dist = dist;
    best = node->point;
  }

  // 决定搜索方向
  bool go_left = query.coords[axis] < node->point.coords[axis];

  // 先搜索更可能包含最近点的子树
  if (go_left) {
    if (node->left != nullptr) {
      ret = NearestNeighborInternal(node->left, query, depth + 1, best, best_dist);
      if (ret != kOk) return ret;
    }

    // 检查另一个子树是否有更近的点
    double axis_dist = std::abs(query.coords[axis] - node->point.coords[axis]);
    if (axis_dist < best_dist && node->right != nullptr) {
      ret = NearestNeighborInternal(node->right, query, depth + 1, best, best_dist);
      if (ret != kOk) return ret;
    }
  } else {
    if (node->right != nullptr) {
      ret = NearestNeighborInternal(node->right, query, depth + 1, best, best_dist);
      if (ret != kOk) return ret;
    }

    // 检查另一个子树是否有更近的点
    double axis_dist = std::abs(query.coords[axis] - node->point.coords[axis]);
    if (axis_dist < best_dist && node->left != nullptr) {
      ret = NearestNeighborInternal(node->left, query, depth + 1, best, best_dist);
      if (ret != kOk) return ret;
    }
  }

  return kOk;
}

Code HNSWGraph::Init() {
  entry_point_ = nullptr;
  if (max_layers_ <= 1 || max_connections_ <= 1) return kInvalidParam;
  if (max_layers_ > 32) return kInvalidParam;
  if (max_connections_ > 32) return kInvalidParam;
  if (max_layers_ < max_connections_) return kInvalidParam;

  return kOk;
}

Code HNSWGraph::Insert(const HNSWPoint& point) {
  int layer = std::min(RandomLayer(), max_layers_ - 1);
  HNSWNode* new_node = new HNSWNode(point, max_layers_);

  if (entry_point_ == nullptr) {
    entry_point_ = new_node;
    return kOk;
  }

  HNSWNode* curr = entry_point_;
  Code ret = kOk;

  // 阶段1: 从顶层向下搜索到目标层
  for (int l = max_layers_ - 1; l > layer; --l) {
    double tmp_dist = 0;
    ret = SearchLayer(point, curr, l, &curr, &tmp_dist);
    if (ret != kOk) return ret;
  }

  // 阶段2: 在目标层及以下建立连接
  for (int l = layer; l >= 0; --l) {
    HNSWNode* nearest = nullptr;
    double tmp_dist = 0;
    ret = SearchLayer(point, curr, l, &nearest, &tmp_dist);
    if (ret != kOk) return ret;

    // 双向建立连接
    new_node->layers[l].push_back(nearest);
    nearest->layers[l].push_back(new_node);

    // 使用启发式算法剪枝新节点的连接
    ret = PruneConnections(new_node, l);
    if (ret != kOk) return ret;

    // 对已存在的邻居节点也进行剪枝
    ret = PruneConnections(nearest, l);
    if (ret != kOk) return ret;

    curr = nearest;  // 为下一层的搜索更新入口
  }
  return kOk;
}

Code HNSWGraph::Search(const HNSWPoint& query, HNSWPoint& best, double& best_dist) {
  if (entry_point_ == nullptr) return kInvalidParam;

  HNSWNode* curr = entry_point_;
  double min_dist = 0;
  for (int l = max_layers_ - 1; l >= 0; --l) {
    Code ret = SearchLayer(query, curr, l, &curr, &min_dist);
    if (ret != kOk) return ret;
  }
  if (curr == nullptr) return kInvalidPlace;
  best = curr->point;
  best_dist = min_dist;
  return kOk;
}

// HNSW (Hierarchical Navigable Small World) 实现
int HNSWGraph::RandomLayer() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);
  return static_cast<int>(-log(dis(gen)));  // 指数分布
}

Code HNSWGraph::SearchLayer(const HNSWPoint& query, HNSWNode* entry, int layer, HNSWNode** nearest_node,
                            double* min_dist) {
  if (entry == nullptr || nearest_node == nullptr || min_dist == nullptr) return kInvalidParam;
  *nearest_node = nullptr;
  *min_dist = std::numeric_limits<double>::max();

  HNSWNode* nearest = entry;
  double tmp_dist = 0;
  Code ret = CalculteDistance(query.coords, nearest->point.coords, &tmp_dist);
  if (ret != kOk) return ret;

  std::unordered_set<HNSWNode*> visited;
  std::vector<HNSWNode*> candidates = {nearest};

  while (!candidates.empty()) {
    HNSWNode* current = candidates.back();
    candidates.pop_back();

    for (HNSWNode* neighbor : current->layers[layer]) {
      if (visited.find(neighbor) == visited.end()) {
        visited.insert(neighbor);
        double dist = 0;
        Code ret = CalculteDistance(query.coords, neighbor->point.coords, &dist);
        if (ret != kOk) return ret;
        if (dist < tmp_dist) {
          tmp_dist = dist;
          nearest = neighbor;
          candidates.push_back(neighbor);
        }
      }
    }
  }
  *nearest_node = nearest;
  *min_dist = tmp_dist;
  return kOk;
}

// 启发式选择邻居算法 (HNSW论文标准实现)
std::vector<HNSWNode*> HNSWGraph::SelectNeighborsHeuristic(const HNSWPoint& point,
                                                           const std::vector<HNSWNode*>& candidates, size_t M) {
  if (candidates.size() <= M) {
    return candidates;
  }

  // 按距离排序候选集
  std::vector<std::pair<double, HNSWNode*>> sorted_candidates;
  sorted_candidates.reserve(candidates.size());

  for (auto* candidate : candidates) {
    double dist = 0;
    Code ret = CalculteDistance(point.coords, candidate->point.coords, &dist);
    if (ret == kOk) {
      sorted_candidates.push_back({dist, candidate});
    }
  }

  std::sort(sorted_candidates.begin(), sorted_candidates.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

  std::vector<HNSWNode*> result;
  result.reserve(M);

  // 启发式选择: 优先选择距离近且方向不重复的邻居
  for (const auto& [dist_to_point, candidate] : sorted_candidates) {
    if (result.size() >= M) break;

    bool should_add = true;

    // 检查候选点是否与已选邻居"过于相似"
    // 如果candidate离某个已选邻居的距离 < candidate到point的距离
    // 说明这个候选点的方向与已选邻居重复
    for (auto* selected : result) {
      double dist_candidate_to_selected = 0;
      Code ret = CalculteDistance(candidate->point.coords, selected->point.coords, &dist_candidate_to_selected);

      if (ret == kOk && dist_candidate_to_selected < dist_to_point) {
        should_add = false;
        break;
      }
    }

    if (should_add) {
      result.push_back(candidate);
    }
  }

  // 如果启发式筛选后不足M个，补充距离最近的
  if (result.size() < M) {
    for (const auto& [dist, candidate] : sorted_candidates) {
      if (result.size() >= M) break;

      // 检查是否已经在结果中
      bool already_added = false;
      for (auto* node : result) {
        if (node == candidate) {
          already_added = true;
          break;
        }
      }

      if (!already_added) {
        result.push_back(candidate);
      }
    }
  }

  return result;
}

// 剪枝连接，使用启发式算法保持最优的max_connections_个连接
Code HNSWGraph::PruneConnections(HNSWNode* node, int layer) {
  if (node == nullptr) return kInvalidParam;
  if (layer < 0 || layer >= max_layers_) return kInvalidParam;

  size_t current_size = node->layers[layer].size();
  if (current_size <= static_cast<size_t>(max_connections_)) {
    return kOk;  // 无需剪枝
  }

  // 使用启发式算法选择最优的max_connections_个邻居
  std::vector<HNSWNode*> pruned =
      SelectNeighborsHeuristic(node->point, node->layers[layer], static_cast<size_t>(max_connections_));

  // 更新邻居列表
  node->layers[layer] = pruned;

  return kOk;
}

}  // namespace base
