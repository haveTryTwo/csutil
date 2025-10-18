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

// 定义点结构
struct KDPoint {
  std::vector<double> coords;
  KDPoint(const std::vector<double>& c) : coords(c) {}
};

// 定义 KD-Tree 节点
struct KDNode {
  KDPoint point;
  KDNode* left;
  KDNode* right;
  KDNode(const KDPoint& p) : point(p), left(nullptr), right(nullptr) {}
};

class KDTree {
 public:
  KDTree(const std::vector<KDPoint>& point);
  ~KDTree();

  Code Init();
  Code NearestNeighbor(const KDPoint& query, KDPoint& best, double& best_dist);

 private:
  Code InitInternal(std::vector<KDPoint>& points, KDNode** build_node, int depth);
  Code NearestNeighborInternal(const KDNode* node, const KDPoint& query, int depth, KDPoint& best, double& best_dist);
  void DestroyTree(KDNode* node);

 private:
  std::vector<KDPoint> points_;
  KDNode* root_ = nullptr;
};

// HNSW (Hierarchical Navigable Small World) 相关结构
// HNSW点结构
struct HNSWPoint {
  std::vector<double> coords;
  HNSWPoint(const std::vector<double>& c) : coords(c) {}
};

// HNSW 图节点
struct HNSWNode {
  HNSWPoint point;
  std::vector<std::vector<HNSWNode*>> layers;  // 每层的邻居
  HNSWNode(const HNSWPoint& p, int max_layers) : point(p), layers(max_layers) {}
};

// HNSW 图
class HNSWGraph {
 public:
  HNSWGraph(int ml, int mc) : max_layers_(ml), max_connections_(mc), entry_point_(nullptr) {}
  ~HNSWGraph() {}

  Code Init();
  int GetMaxLevel() const { return max_layers_; }

  // 插入点
  Code Insert(const HNSWPoint& point);

  // 搜索最近邻
  Code Search(const HNSWPoint& query, HNSWPoint& best, double& best_dist);

 private:
  // 随机生成层数
  int RandomLayer();

  // 在某一层搜索最近邻
  Code SearchLayer(const HNSWPoint& query, HNSWNode* entry, int layer, HNSWNode** nearest_node, double* min_dist);

  // 启发式选择邻居 (标准HNSW算法)
  std::vector<HNSWNode*> SelectNeighborsHeuristic(const HNSWPoint& point, const std::vector<HNSWNode*>& candidates,
                                                  size_t M);

  // 剪枝连接，保持连接数不超过max_connections
  Code PruneConnections(HNSWNode* node, int layer);

 private:
  int max_layers_;         // 最大层数
  int max_connections_;    // 每层最大连接数
  HNSWNode* entry_point_;  // 入口点
};

}  // namespace base
#endif
