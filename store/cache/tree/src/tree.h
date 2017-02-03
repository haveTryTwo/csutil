// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef STORE_CACHE_TREE_H_
#define STORE_CACHE_TREE_H_

#include <string>
#include <vector>

#include <stdint.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>

#include "base/status.h"

#include "tree_model.pb.h"

namespace store
{

const std::string kRootPath     = "/";
const std::string kWhiteChar    = " \t";
const char kFileDelim           = '/';

class Tree
{
    public:
        Tree();
        ~Tree();

        base::Code Init();
        base::Code Init(uint64_t trx_id, const std::string &dump_msg);

        base::Code Add(const std::string &path, const std::string &value);
        base::Code Delete(int64_t version, const std::string &path);
        base::Code Modify(int64_t version, const std::string &path, const std::string &new_value);
        base::Code Get(const std::string &path, tree_model::TreeNodeHead *tree_node_head);
        base::Code GetChildren(const std::string &path, std::vector<std::string> *nodes);

        void Print();

    private:
        base::Code GetNamesOfPath(const std::string &path, std::vector<std::string> *names);
        base::Code InitNowTreeNode(const std::string &node_name, const std::string &value, tree_model::TreeNode *tree_node);
        base::Code FindNode(::google::protobuf::RepeatedPtrField<tree_model::TreeNode> &tree_list, const std::string &name, tree_model::TreeNode **tree_node);

    private:
        uint64_t trx_id_;
        tree_model::TreeNode tree_;
};

}

#endif
