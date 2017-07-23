// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>

#include "base/trie.h"
#include "base/util.h"

namespace base
{

Trie::Trie() : root_(NULL), is_init_(false)
{/*{{{*/
}/*}}}*/

Trie::~Trie()
{/*{{{*/
    if (root_ != NULL)
    {
        Destroy(root_);
        root_ = NULL;
    }
}/*}}}*/

Code Trie::Put(const std::string &key)
{/*{{{*/
    if (!is_init_) return kNotInit;

    bool is_lower = false;
    Code ret = CheckIsLowerCaseLetter(key, &is_lower);
    if (ret != kOk) return ret;
    if (!is_lower) return kNotAllLowerCaseLetter;

    TrieItem *cur_item = root_;
    cur_item->frequency++;
    for (uint32_t i = 0; i < key.size(); ++i)
    {
        uint32_t pos = key.data()[i] - 'a';
        if (cur_item->index[pos] == NULL)
        {
            TrieItem *tmp_item  = new TrieItem();
            if (tmp_item == NULL) return kNewFailed;
            tmp_item->Clear();

            cur_item->index[pos] = tmp_item;
        }

        cur_item = cur_item->index[pos];
        cur_item->frequency++;
    }

    cur_item->full_word_frequency++;

    return ret;
}/*}}}*/

Code Trie::Get(const std::string &key, uint32_t *full_word_frequency)
{/*{{{*/
    if (full_word_frequency == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    bool is_lower = false;
    Code ret = CheckIsLowerCaseLetter(key, &is_lower);
    if (ret != kOk) return ret;
    if (!is_lower) return kNotAllLowerCaseLetter;

    TrieItem *cur_item = root_;
    for (uint32_t i = 0; i < key.size(); ++i)
    {
        uint32_t pos = key.data()[i] - 'a';
        if (cur_item->index[pos] == NULL) return kNotFound;

        cur_item = cur_item->index[pos];
    }

    if (cur_item->full_word_frequency <= 0) return kNotFound;

    *full_word_frequency = cur_item->frequency;

    return ret;
}/*}}}*/

Code Trie::Del(const std::string &key)
{/*{{{*/
    if (!is_init_) return kNotInit;

    uint32_t full_word_frequency = 0;
    Code ret = Get(key, &full_word_frequency);
    if (ret != kOk) return ret;
    assert(full_word_frequency > 0);

    TrieItem *cur_item = root_;
    cur_item->frequency--;
    for (uint32_t i = 0; i < key.size(); ++i)
    {/*{{{*/
        uint32_t pos = key.data()[i] - 'a';
        assert(cur_item->index[pos] != NULL);

        cur_item = cur_item->index[pos];
        cur_item->frequency--;
    }/*}}}*/

    cur_item->full_word_frequency--;

    return ret;
}/*}}}*/

Code Trie::ToString(std::string *info)
{/*{{{*/
    if (info == NULL) return kInvalidParam;
    if (!is_init_) return kNotInit;

    Code ret = ToString("", root_, info);
    return ret;
}/*}}}*/

Code Trie::ToString(const std::string &prefix, const TrieItem *trie_item, std::string *info)
{/*{{{*/
    if (trie_item == NULL || info == NULL) return kInvalidParam;

    uint32_t index_size = sizeof(trie_item->index)/sizeof(trie_item->index[0]);
    for (uint32_t i = 0; i < index_size; ++i)
    {
        if (trie_item->index[i] != NULL)
        {
            std::string cur_str = prefix;
            cur_str.append(1, i+'a');
            if (trie_item->index[i]->full_word_frequency > 0)
            {
                info->append(cur_str);
                info->append(1, '\n');
            }

            ToString(cur_str, trie_item->index[i], info);
        }
    }

    return kOk;
}/*}}}*/

Code Trie::Init()
{/*{{{*/
    root_ = new TrieItem();
    if (root_ == NULL) return kNewFailed;
    root_->Clear();

    is_init_ = true;

    return kOk;
}/*}}}*/

Code Trie::Destroy(TrieItem *trie_item)
{/*{{{*/
    if (trie_item == NULL) return kInvalidParam;

    uint32_t index_size = sizeof(trie_item->index)/sizeof(trie_item->index[0]);
    for (uint32_t i = 0; i < index_size; ++i)
    {
        if (trie_item->index[i] != NULL)
        {
            Destroy(trie_item->index[i]);
            trie_item->index[i] = NULL;
        }
    }

    delete trie_item;

    return kOk;
}/*}}}*/

}
