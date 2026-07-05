// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "agent/src/kv_store.h"

#include "leveldb/db.h"
#include "leveldb/iterator.h"
#include "leveldb/options.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace book_agent {

KvStore::KvStore() : db_(NULL) { /*{{{*/ } /*}}}*/

KvStore::~KvStore() { /*{{{*/
  if (db_ != NULL) {
    delete db_;
    db_ = NULL;
  }
} /*}}}*/

base::Code KvStore::Init(const std::string &db_path) { /*{{{*/
  if (db_path.empty()) return base::kInvalidParam;

  leveldb::Options options;
  options.create_if_missing = true;
  options.compression = leveldb::kNoCompression;  // demo 关闭压缩，减少外部依赖

  leveldb::DB *db = NULL;
  leveldb::Status status = leveldb::DB::Open(options, db_path, &db);
  if (!status.ok()) return base::kOpenError;

  db_ = db;

  return base::kOk;
} /*}}}*/

base::Code KvStore::Put(const std::string &key, const std::string &value) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;

  leveldb::Status status = db_->Put(leveldb::WriteOptions(), key, value);
  if (!status.ok()) return base::kWriteError;

  return base::kOk;
} /*}}}*/

base::Code KvStore::Get(const std::string &key, std::string *value) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;
  if (value == NULL) return base::kInvalidParam;

  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, value);
  if (status.IsNotFound()) return base::kNotFound;
  if (!status.ok()) return base::kReadError;

  return base::kOk;
} /*}}}*/

base::Code KvStore::Delete(const std::string &key) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;

  leveldb::Status status = db_->Delete(leveldb::WriteOptions(), key);
  if (!status.ok()) return base::kWriteError;

  return base::kOk;
} /*}}}*/

base::Code KvStore::Scan(const std::string &prefix, const std::string &start_after, uint32_t limit,
                                std::vector<std::string> *keys, std::vector<std::string> *values,
                                std::string *next_cursor) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;
  if (limit == 0 || keys == NULL || values == NULL || next_cursor == NULL) return base::kInvalidParam;

  keys->clear();
  values->clear();
  next_cursor->clear();

  leveldb::Iterator *it = db_->NewIterator(leveldb::ReadOptions());

  // 定位起点：优先游标，其次前缀，最后从头
  if (!start_after.empty()) {
    it->Seek(start_after);
    if (it->Valid() && it->key().ToString() == start_after) it->Next();  // 游标自身不含
  } else if (!prefix.empty()) {
    it->Seek(prefix);
  } else {
    it->SeekToFirst();
  }

  uint32_t count = 0;
  for (; it->Valid() && count < limit; it->Next()) {
    std::string key = it->key().ToString();
    if (!prefix.empty() && key.compare(0, prefix.size(), prefix) != 0) break;  // 已越过前缀范围
    keys->push_back(key);
    values->push_back(it->value().ToString());
    ++count;
  }

  // 满页且后面仍有数据时才给游标
  if (count == limit && it->Valid()) *next_cursor = keys->back();

  delete it;
  return base::kOk;
} /*}}}*/

}  // namespace book_agent
