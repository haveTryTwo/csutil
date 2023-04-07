// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "proto/pb_manage.h"

namespace proto {

PBManage::PBManage(const std::string &pb_dir, const std::string &pb_file_name)
    : pb_dir_(pb_dir),
      pb_file_name_(pb_file_name),
      source_tree_(),
      collector_(),
      importer_(&source_tree_, &collector_) { /*{{{*/
  source_tree_.MapPath("", pb_dir_);
  file_descriptor_ = NULL;
} /*}}}*/

PBManage::~PBManage() { /*{{{*/
} /*}}}*/

base::Code PBManage::Init(const std::vector<std::string> &type_names) { /*{{{*/
  file_descriptor_ = importer_.Import(pb_file_name_);
  if (file_descriptor_ == NULL) {
    fprintf(stderr, "Failed to import %s\n", pb_file_name_.c_str());
    return base::kImportFailed;
  }

  std::vector<std::string>::const_iterator it;
  for (it = type_names.begin(); it != type_names.end(); ++it) {
    const google::protobuf::Descriptor *descriptor = file_descriptor_->FindMessageTypeByName(*it);
    if (descriptor == NULL) {
      return base::kInvalidParam;
    }

    const google::protobuf::Message *msg = factory_.GetPrototype(descriptor);
    if (msg == NULL) {
      return base::kInvalidParam;
    }

    msg_proto_.insert(std::pair<std::string, const google::protobuf::Message *>(*it, msg));
  }

  return base::kOk;
} /*}}}*/

google::protobuf::Message *PBManage::NewMsg(const std::string &type_name) { /*{{{*/
  std::map<std::string, const google::protobuf::Message *>::iterator it =
      msg_proto_.find(type_name);
  if (it == msg_proto_.end()) {
    return NULL;
  }

  return it->second->New();
} /*}}}*/

}  // namespace proto
