// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PB_UTIL_H_
#define PB_UTIL_H_

#include <deque>
#include <string>

#include <stdint.h>

#include <google/protobuf/message.h>

#include "base/status.h"

#include "rapidjson/document.h"

namespace proto {

enum DiffStatus { /*{{{*/
                  kEqual = 0,
                  kPBTypeNotEqual = 1,
                  kValueNotEqual = 2,
                  kArraySizeNotEqual = 3,
                  kFirstValueEmpty = 4,
                  kSecondValueEmpty = 5,
}; /*}}}*/

enum DiffType { /*{{{*/
                kDefault = 0,
                kInt32 = 1,
                kUInt32 = 2,
                kInt64 = 3,
                kUInt64 = 4,
                kDouble = 5,
                kFloat = 6,
                kBool = 7,
                kEnum = 8,
                kString = 9,
                kObject = 10,  // When one party's data if empty
};                             /*}}}*/

struct DiffValue { /*{{{*/
  int first_value_int32;
  int second_value_int32;

  uint32_t first_value_uint32;
  uint32_t second_value_uint32;

  int64_t first_value_int64;
  int64_t second_value_int64;

  uint64_t first_value_uint64;
  uint64_t second_value_uint64;

  double first_value_double;
  double second_value_double;

  float first_value_float;
  float second_value_float;

  bool first_value_bool;
  bool second_value_bool;

  int first_value_enum;
  int second_value_enum;

  std::string first_value_string;
  std::string second_value_string;

  DiffValue()
      : first_value_int32(0),
        second_value_int32(0),
        first_value_uint32(0),
        second_value_uint32(0),
        first_value_int64(0),
        second_value_int64(0),
        first_value_uint64(0),
        second_value_uint64(0),
        first_value_double(0),
        second_value_double(0),
        first_value_float(0),
        second_value_float(0),
        first_value_bool(false),
        second_value_bool(false),
        first_value_enum(0),
        second_value_enum(0) {}

}; /*}}}*/

struct DiffContent { /*{{{*/
  DiffStatus status;

  std::string first_type;  // Scene of different protobuf type
  std::string second_type;

  std::string key;  // NOTE:htt, key of filed
  DiffType type;    // NOTE:htt, key of field

  int first_array_size;  // Scene of different size of array
  int second_array_size;

  DiffValue value;

  DiffContent() : status(kEqual), type(kDefault), first_array_size(0), second_array_size(0) {}

  void Print();
  bool operator==(const DiffContent &other);
}; /*}}}*/

base::Code PBDiffWithOutExtension(const ::google::protobuf::Message &msg_first,
                                  const ::google::protobuf::Message &msg_second, bool *is_diff,
                                  std::deque<DiffContent> *diff_contents);

/**
 * NOTE: Initialize the corresponding value to the default value
 *  The init_keys must be key of leaf node
 */
base::Code InitJsonValue(const std::string &json, const std::set<std::string> init_keys_list,
                         std::string *dst_json);

base::Code InitJsonValue(const rapidjson::Value &json, const std::set<std::string> init_keys_list,
                         rapidjson::Value *dst_json, rapidjson::Value::AllocatorType &alloc);

}  // namespace proto

#endif
