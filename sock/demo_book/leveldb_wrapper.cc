// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/demo_book/leveldb_wrapper.h"

#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace book_mgr {

LevelDbWrapper::LevelDbWrapper() : db_(NULL) { /*{{{*/ } /*}}}*/

LevelDbWrapper::~LevelDbWrapper() { /*{{{*/
  if (db_ != NULL) {
    delete db_;
    db_ = NULL;
  }
} /*}}}*/

base::Code LevelDbWrapper::Init(const std::string &db_path) { /*{{{*/
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

base::Code LevelDbWrapper::Put(const std::string &key, const std::string &value) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;

  leveldb::Status status = db_->Put(leveldb::WriteOptions(), key, value);
  if (!status.ok()) return base::kWriteError;

  return base::kOk;
} /*}}}*/

base::Code LevelDbWrapper::Get(const std::string &key, std::string *value) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;
  if (value == NULL) return base::kInvalidParam;

  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, value);
  if (status.IsNotFound()) return base::kNotFound;
  if (!status.ok()) return base::kReadError;

  return base::kOk;
} /*}}}*/

base::Code LevelDbWrapper::Delete(const std::string &key) { /*{{{*/
  if (db_ == NULL) return base::kNotInit;

  leveldb::Status status = db_->Delete(leveldb::WriteOptions(), key);
  if (!status.ok()) return base::kWriteError;

  return base::kOk;
} /*}}}*/

}  // namespace book_mgr
