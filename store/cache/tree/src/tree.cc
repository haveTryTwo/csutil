// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/log.h"
#include "base/util.h"
#include "base/time.h"

#include "tree.h"

namespace store
{

Tree::Tree() : trx_id_(0)
{
}

Tree::~Tree()
{
}

base::Code Tree::Init()
{/*{{{*/
    trx_id_ = 0;

    return Add(kRootPath, "");
}/*}}}*/

base::Code Tree::Init(uint64_t trx_id, const std::string &dump_msg)
{/*{{{*/
    if (dump_msg.empty()) return base::kInvalidParam;

    trx_id_ = trx_id;
    if (!tree_.ParseFromString(dump_msg))
    {
        base::LOG_ERR("Failed to parse dump msg to tree! version:%llu, dump_msg:%s",
                trx_id_, dump_msg.c_str());
        return base::kInvalidPbMessage;
    }

    return base::kOk;
}/*}}}*/

base::Code Tree::Add(const std::string &path, const std::string &value)
{/*{{{*/
    if (path.empty()) return base::kInvalidParam;

    std::vector<std::string> names;
    base::Code ret = GetNamesOfPath(path, &names);
    if (ret != base::kOk)
    {
        base::LOG_ERR("Failed to get names of path:%s, ret:%d", path.c_str(), ret);
        return ret;
    }

    // Add root node: '/'
    if (names.size() == 1)
    {/*{{{*/
        ret = InitNowTreeNode(names[0], value, &tree_);
        if (ret != base::kOk)
        {
            base::LOG_ERR("Failed to create root node:%s! ret:%d", path.c_str(), ret);
            return ret;
        }

        return ret;
    }/*}}}*/

    // TODO: add children node
    int depth = 1;
    tree_model::TreeNode *father_tree_node = &tree_;
    while (true)
    {/*{{{*/
        if (depth >= (int)names.size())
        {
            base::LOG_ERR("Invalid depth:%d, Faield to create node path:%s", depth, path.c_str());
            return base::kInvalidPath;
        }

        if (father_tree_node->children_nodes_size() == 0)
        {/*{{{*/
            if (depth == ((int)names.size() - 1))
            {
                tree_model::TreeNode tmp_node;
                ret = InitNowTreeNode(names[depth], value, &tmp_node);
                if (ret != base::kOk)
                {
                    base::LOG_ERR("Failed to create node:%s! ret:%d", names[depth].c_str(), ret);
                    return ret;
                }

                tree_model::TreeNode *child_node = father_tree_node->add_children_nodes();
                *child_node = tmp_node;

                tree_model::TreeNodeHead *father_head = father_tree_node->mutable_head();
                father_head->set_children_version(father_head->children_version()+1);

                break;
            }
            else
            {
                base::LOG_ERR("Father node:%s not exist! path:%s", names[depth].c_str(), path.c_str());
                return base::kInvalidPath;
            }
        }/*}}}*/

        ::google::protobuf::RepeatedPtrField<tree_model::TreeNode> *childrens = father_tree_node->mutable_children_nodes();
        tree_model::TreeNode *cur_node = NULL;
        ret = FindNode(*childrens, names[depth], &cur_node);
        if (ret == base::kNotFound)
        {/*{{{*/
            if (depth == ((int)names.size() - 1))
            {
                tree_model::TreeNode tmp_node;
                ret = InitNowTreeNode(names[depth], value, &tmp_node);
                if (ret != base::kOk)
                {
                    base::LOG_ERR("Failed to create node:%s! ret:%d", names[depth].c_str(), ret);
                    return ret;
                }

                tree_model::TreeNode *child_node = father_tree_node->add_children_nodes();
                *child_node = tmp_node;

                tree_model::TreeNodeHead *father_head = father_tree_node->mutable_head();
                father_head->set_children_version(father_head->children_version()+1);

                // TODO: sort the children_nodes by name
                break;
            }
            else
            {
                base::LOG_ERR("Father node:%s not exist! path:%s", names[depth].c_str(), path.c_str());
                return base::kInvalidPath;
            }
        }/*}}}*/
        else if (ret != base::kOk)
        {/*{{{*/
            base::LOG_ERR("Failed to find node:%s, path:%s, ret:%d",
                    names[depth].c_str(), path.c_str(), ret);
            return ret;
        }/*}}}*/

        if (depth == ((int)names.size() - 1))
        {
            base::LOG_ERR("Node:%s exist!", path.c_str());
            return base::kNodeExist;
        }

        father_tree_node = cur_node;
        ++depth;
    }/*}}}*/
    
    return ret;
}/*}}}*/

void Tree::Print()
{/*{{{*/
    fprintf(stderr, "%s\n", tree_.DebugString().c_str());
}/*}}}*/

base::Code Tree::GetNamesOfPath(const std::string &path, std::vector<std::string> *names)
{/*{{{*/
    if (path.empty() || names == NULL) return base::kInvalidParam;

    std::string real_path;
    base::Code ret = base::Trim(path, kWhiteChar, &real_path);
    if (ret != base::kOk)
    {
        base::LOG_ERR("Failed to trim path:%s, ret:%d", path.c_str(), ret);
        return ret;
    }

    if (real_path.empty()) return base::kInvalidPath;

    if (real_path.data()[0] != kFileDelim)
    {
        base::LOG_ERR("Invalid path:%s that not contain root node:%s", path.c_str(), kRootPath.c_str());
        return base::kInvalidPath;
    }

    names->push_back(kRootPath);
    
    const char *start = real_path.data() + 1;
    for (int i = 1; i < (int)real_path.size(); ++i)
    {
        const char *pos = strchr(start, kFileDelim);
        if (pos == NULL)
        {
            if (*start == '\0')
            {
                base::LOG_ERR("Invalid path:%s", path.c_str());
                return base::kInvalidPath;
            }

            names->push_back(start);
            break;
        }

        if (pos == start)
        {
            base::LOG_ERR("Invalid path:%s", path.c_str());
            return base::kInvalidPath;
        }

        names->push_back(std::string(start, pos-start));
        start = pos+1;
    }

    return base::kOk;
}/*}}}*/

base::Code Tree::InitNowTreeNode(const std::string &node_name, const std::string &value, tree_model::TreeNode *tree_node)
{/*{{{*/
    if (node_name.empty() || tree_node == NULL) return base::kInvalidParam;
    
    if (tree_node->mutable_head()->has_name())
    {
        base::LOG_ERR("Node:%s has exist", tree_node->mutable_head()->name().c_str());
        return base::kNodeExist;
    }

    tree_model::TreeNodeHead tmp_head;
    ++trx_id_;
    tmp_head.set_trx_id(trx_id_);
    tmp_head.set_name(node_name);
    tmp_head.set_value(value);
    tmp_head.set_version(0);
    tmp_head.set_children_version(0);
    struct timeval now_tm;
    base::Code base_r = base::Time::GetTime(&now_tm);
    if (base_r != base::kOk)
    {
        base::LOG_ERR("Failed to get time, ret:%d", base_r);
        return base_r;
    }
    tmp_head.set_create_time(now_tm.tv_sec);
    tmp_head.set_modify_time(now_tm.tv_sec);

    tree_model::TreeNodeHead *tree_node_head = tree_node->mutable_head();
    *tree_node_head = tmp_head;

    return base::kOk;
}/*}}}*/

base::Code Tree::FindNode(::google::protobuf::RepeatedPtrField<tree_model::TreeNode> &tree_list, const std::string &name, tree_model::TreeNode **tree_node)
{/*{{{*/
    if (name.empty() || tree_node == NULL) return base::kInvalidParam;
    
    base::LOG_INFO("cur tree list size:%d, try to find name:%s", tree_list.size(), name.c_str());
    for (int i = 0; i < tree_list.size(); ++i)
    {
        const tree_model::TreeNodeHead &head = tree_list.Get(i).head();
        base::LOG_INFO("cur index:%d, cur name:%s, try to find name:%s", i, head.name().c_str(), 
                       name.c_str());
        if (head.name().compare(name) == 0)
        {
            *tree_node = tree_list.Mutable(i);
            return base::kOk;
        }
    }

    return base::kNotFound;
}/*}}}*/

}

#ifdef _TREE_MAIN_TEST_
int main(int argc, char *argv[])
{/*{{{*/
    using namespace base;
    using namespace store;

//    SetLogLevel(base::kInfoLevel);

    Tree tree;
    Code ret = tree.Init();
    if (ret != kOk)
    {
        LOG_ERR("Failed to init tree!");
        return ret;
    }
    tree.Print();

    // Test add root node; this will fail
    std::string path = "/";
    std::string value = "123";

    ret = tree.Add(path, value);
    assert(ret == kNodeExist);

    // Test add new node c1
    path = "/c1";
    value = "value_c1";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add new node b1
    path = "/b1";
    value = "value_b1";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add new node a1
    path = "/a1";
    value = "value_a1";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add new node a11 of node a1
    path = "/a1/a11";
    value = "value_a11";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add new node a22 of node a1
    path = "/a1/a22";
    value = "value_a22";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add new node a111 of node a11
    path = "/a1/a11/a111";
    value = "value_a111";
    ret = tree.Add(path, value);
    assert(ret == kOk);
    tree.Print();

    // Test add exist node a111 of node a11
    path = "/a1/a11/a111";
    value = "value_a111";
    ret = tree.Add(path, value);
    assert(ret == kNodeExist);

    // Test add parent node that is not exist nnnn 
    path = "/a1/ann/aoo";
    value = "value_aoo";
    ret = tree.Add(path, value);
    assert(ret == kInvalidPath);

    // Test add node that not contains root node
    path = "d1";
    value = "value_d1";
    ret = tree.Add(path, value);
    assert(ret == kInvalidPath);

    return 0;
}/*}}}*/
#endif
