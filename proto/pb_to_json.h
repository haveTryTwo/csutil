// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PB_TO_JSON_H_
#define PB_TO_JSON_H_

#include <string>

#include <stdint.h>

#include <google/protobuf/message.h>
#include "rapidjson/document.h"

#include "base/status.h"

namespace proto {

base::Code PBToJsonWithOutExtension(const ::google::protobuf::Message &msg, std::string *json);
base::Code JsonToPBWithOutExtension(const std::string &json, ::google::protobuf::Message *msg);

base::Code PBToJsonWithOutExtension(const ::google::protobuf::Message &msg, rapidjson::Value::AllocatorType &alloc,
                                    rapidjson::Value *json);
base::Code JsonToPBWithOutExtension(const rapidjson::Value &json, ::google::protobuf::Message *msg);

}  // namespace proto

#endif
