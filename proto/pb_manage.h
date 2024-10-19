// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PB_MANAGE_H_
#define PB_MANAGE_H_

#include <string>

#include <stdint.h>
#include <stdio.h>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/common.h>

#include "base/status.h"

namespace proto {

class ErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
 public:
  ErrorCollector() : warnings_(0), errors_(0) {}

  virtual ~ErrorCollector() {}

  virtual void AddError(const std::string &filename, int line, int column, const std::string &message) {
    ++errors_;
    fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
  }

  virtual void AddWarning(const std::string &filename, int line, int column, const std::string &message) {
    ++warnings_;
    fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
  }

  uint32_t errors() const { return errors_; }
  uint32_t warnings() const { return warnings_; }

 private:
  uint32_t warnings_;
  uint32_t errors_;
};

class PBManage { /*{{{*/
 public:
  PBManage(const std::string &pb_dir, const std::string &pb_file_name);
  ~PBManage();

  base::Code Init(const std::vector<std::string> &type_names);

  google::protobuf::Message *NewMsg(const std::string &type_name);

 private:
  std::string pb_dir_;
  std::string pb_file_name_;

  std::map<std::string, const google::protobuf::Message *> msg_proto_;

  google::protobuf::compiler::DiskSourceTree source_tree_;
  ErrorCollector collector_;
  google::protobuf::compiler::Importer importer_;
  google::protobuf::DynamicMessageFactory factory_;
  const google::protobuf::FileDescriptor *file_descriptor_;
}; /*}}}*/

}  // namespace proto

#endif
