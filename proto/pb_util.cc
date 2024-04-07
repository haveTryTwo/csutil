// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/common.h"
#include "pb_util.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "proto/pb_to_json.h"

namespace proto {

#define DIFF_OPERATION(field_type, field_type_str, pb_field_type_str) \
  {                                                                   \
    diff_content.type = field_type;                                   \
    if (is_first_value_exist) {                                       \
      diff_content.value.first_value_##field_type_str =               \
          reflection->Get##pb_field_type_str(msg_first, field_desc);  \
    }                                                                 \
                                                                      \
    if (is_second_value_exist) {                                      \
      diff_content.value.second_value_##field_type_str =              \
          reflection->Get##pb_field_type_str(msg_second, field_desc); \
    }                                                                 \
                                                                      \
    if (is_first_value_exist == is_second_value_exist) {              \
      if (diff_content.value.first_value_##field_type_str ==          \
          diff_content.value.second_value_##field_type_str) {         \
        break;                                                        \
      } else {                                                        \
        *is_diff = true;                                              \
        diff_content.status = kValueNotEqual;                         \
      }                                                               \
    } else {                                                          \
      *is_diff = true;                                                \
      if (is_first_value_exist == false) {                            \
        diff_content.status = kFirstValueEmpty;                       \
      } else {                                                        \
        diff_content.status = kSecondValueEmpty;                      \
      }                                                               \
    }                                                                 \
    diff_contents->push_back(diff_content);                           \
                                                                      \
    break;                                                            \
  }

#define DIFF_ARRAY_OPERATION(field_type, field_type_str, cpp_type, pb_field_type_str) \
  {                                                                                   \
    diff_content.type = field_type;                                                   \
    if (first_field_size == second_field_size) {                                      \
      for (int j = 0; j < first_field_size; ++j) {                                    \
        cpp_type first_value =                                                        \
            reflection->GetRepeated##pb_field_type_str(msg_first, field_desc, j);     \
        cpp_type second_value =                                                       \
            reflection->GetRepeated##pb_field_type_str(msg_second, field_desc, j);    \
        if (first_value != second_value) {                                            \
          *is_diff = true;                                                            \
          char tmp_buf[base::kSmallBufLen] = {0};                                     \
          snprintf(tmp_buf, sizeof(tmp_buf) - 1, ".%d", j);                           \
          diff_content.key = diff_content.key + tmp_buf;                              \
          diff_content.status = kValueNotEqual;                                       \
          diff_content.value.first_value_##field_type_str = first_value;              \
          diff_content.value.second_value_##field_type_str = second_value;            \
                                                                                      \
          diff_contents->push_back(diff_content);                                     \
          break;                                                                      \
        }                                                                             \
      }                                                                               \
    } else {                                                                          \
      *is_diff = true;                                                                \
      diff_content.status = kArraySizeNotEqual;                                       \
      diff_content.first_array_size = first_field_size;                               \
      diff_content.second_array_size = second_field_size;                             \
      diff_contents->push_back(diff_content);                                         \
    }                                                                                 \
    break;                                                                            \
  }

static base::Code IsValueExist(const ::google::protobuf::Message &msg,
                               const ::google::protobuf::Reflection *reflection,
                               const ::google::protobuf::FieldDescriptor *field_desc,
                               bool *is_value_exist);

static base::Code PBDiffWithOutExtension(const ::google::protobuf::Message &msg_first,
                                         const ::google::protobuf::Message &msg_second,
                                         const std::string &key_prefix, bool *is_diff,
                                         std::deque<DiffContent> *diff_contents);

void DiffContent::Print() { /*{{{*/
  if (status == kEqual) {
  } else if (status == kPBTypeNotEqual) {
    fprintf(stderr, "first_type:%s, second_type:%s\n", first_type.c_str(), second_type.c_str());
  } else if (status == kValueNotEqual) {
    fprintf(stderr, "key:%s\n", key.c_str());
    fprintf(stderr, "type:%d\n", type);
    switch (type) { /*{{{*/
      case kInt32:
        fprintf(stderr, "[int32] first value:%d, second value:%d\n", value.first_value_int32,
                value.second_value_int32);
        break;
      case kUInt32:
        fprintf(stderr, "[uint32] first value:%u, second value:%u\n", value.first_value_uint32,
                value.second_value_uint32);
        break;
      case kInt64:
        fprintf(stderr, "[int64] first value:%lld, second value:%lld\n", value.first_value_int64,
                value.second_value_int64);
        break;
      case kUInt64:
        fprintf(stderr, "[uint64] first value:%llu, second value:%llu\n", value.first_value_uint64,
                value.second_value_uint64);
        break;
      case kDouble:
        fprintf(stderr, "[double] first value:%lf, second value:%lf\n", value.first_value_double,
                value.second_value_double);
        break;
      case kFloat:
        fprintf(stderr, "[float] first value:%f, second value:%f\n", value.first_value_float,
                value.second_value_float);
        break;
      case kBool:
        fprintf(stderr, "[bool] first value:%d, second value:%d\n", value.first_value_bool,
                value.second_value_bool);
        break;
      case kEnum:
        fprintf(stderr, "[enum] first value:%d, second value:%d\n", value.first_value_enum,
                value.second_value_enum);
        break;
      case kString:
        fprintf(stderr, "[string] first value:%s, second value:%s\n",
                value.first_value_string.c_str(), value.second_value_string.c_str());
        break;
      default:
        fprintf(stderr, "invalid type:%d\n", type);
        break;
    } /*}}}*/
  } else if (status == kArraySizeNotEqual) {
    fprintf(stderr, "key:%s\n", key.c_str());
    fprintf(stderr, "type:%d\n", type);
    fprintf(stderr, "first array size:%d, second array size:%d\n", first_array_size,
            second_array_size);
  } else if (status == kFirstValueEmpty) {
    fprintf(stderr, "key:%s\n", key.c_str());
    fprintf(stderr, "type:%d\n", type);
    fprintf(stderr, "first value empty\n");
    switch (type) { /*{{{*/
      case kInt32:
        fprintf(stderr, "[int32] second value:%d\n", value.second_value_int32);
        break;
      case kUInt32:
        fprintf(stderr, "[uint32] second value:%u\n", value.second_value_uint32);
        break;
      case kInt64:
        fprintf(stderr, "[int64] second value:%lld\n", value.second_value_int64);
        break;
      case kUInt64:
        fprintf(stderr, "[uint64] second value:%llu\n", value.second_value_uint64);
        break;
      case kDouble:
        fprintf(stderr, "[double] second value:%lf\n", value.second_value_double);
        break;
      case kFloat:
        fprintf(stderr, "[float] second value:%f\n", value.second_value_float);
        break;
      case kBool:
        fprintf(stderr, "[bool] second value:%d\n", value.second_value_bool);
        break;
      case kEnum:
        fprintf(stderr, "[enum] second value:%d\n", value.second_value_enum);
        break;
      case kString:
        fprintf(stderr, "[string] second value:%s\n", value.second_value_string.c_str());
        break;
      case kObject:
        fprintf(stderr, "[object] second value is object\n");
        break;
      default:
        fprintf(stderr, "invalid type:%d\n", type);
        break;
    } /*}}}*/
  } else if (status == kSecondValueEmpty) {
    fprintf(stderr, "key:%s\n", key.c_str());
    fprintf(stderr, "type:%d\n", type);
    switch (type) { /*{{{*/
      case kInt32:
        fprintf(stderr, "[int32] first value:%d\n", value.first_value_int32);
        break;
      case kUInt32:
        fprintf(stderr, "[uint32] first value:%u\n", value.first_value_uint32);
        break;
      case kInt64:
        fprintf(stderr, "[int64] first value:%lld\n", value.first_value_int64);
        break;
      case kUInt64:
        fprintf(stderr, "[uint64] first value:%llu\n", value.first_value_uint64);
        break;
      case kDouble:
        fprintf(stderr, "[double] first value:%lf\n", value.first_value_double);
        break;
      case kFloat:
        fprintf(stderr, "[float] first value:%f\n", value.first_value_float);
        break;
      case kBool:
        fprintf(stderr, "[bool] first value:%d\n", value.first_value_bool);
        break;
      case kEnum:
        fprintf(stderr, "[enum] first value:%d\n", value.first_value_enum);
        break;
      case kString:
        fprintf(stderr, "[string] first value:%s\n", value.first_value_string.c_str());
        break;
      case kObject:
        fprintf(stderr, "[object] first value is object\n");
        break;
      default:
        fprintf(stderr, "invalid type:%d\n", type);
        break;
    } /*}}}*/
    fprintf(stderr, "second value empty\n");
  }
} /*}}}*/

bool DiffContent::operator==(const DiffContent &other) { /*{{{*/
  if (status != other.status) return false;

  if (status == kEqual) {
    return true;
  } else if (status == kPBTypeNotEqual) {
    return first_type == other.first_type && second_type == other.second_type;
  } else if (status == kValueNotEqual) {
    if (key != other.key) return false;
    if (type != other.type) return false;

    switch (type) { /*{{{*/
      case kInt32:
        return value.first_value_int32 == other.value.first_value_int32 &&
               value.second_value_int32 == other.value.second_value_int32;
      case kUInt32:
        return value.first_value_uint32 == other.value.first_value_uint32 &&
               value.second_value_uint32 == other.value.second_value_uint32;
      case kInt64:
        return value.first_value_int64 == other.value.first_value_int64 &&
               value.second_value_int64 == other.value.second_value_int64;
      case kUInt64:
        return value.first_value_uint64 == other.value.first_value_uint64 &&
               value.second_value_uint64 == other.value.second_value_uint64;
      case kDouble:
        return value.first_value_double == other.value.first_value_double &&
               value.second_value_double == other.value.second_value_double;
      case kFloat:
        return value.first_value_float == other.value.first_value_float &&
               value.second_value_float == other.value.second_value_float;
      case kBool:
        return value.first_value_bool == other.value.first_value_bool &&
               value.second_value_bool == other.value.second_value_bool;
      case kEnum:
        return value.first_value_enum == other.value.first_value_enum &&
               value.second_value_enum == other.value.second_value_enum;
      case kString:
        return value.first_value_string == other.value.first_value_string &&
               value.second_value_string == other.value.second_value_string;
      default:
        fprintf(stderr, "invalid type:%d\n", type);
        return false;
    } /*}}}*/
  } else if (status == kArraySizeNotEqual) {
    if (key != other.key) return false;
    if (type != other.type) return false;
    return first_array_size == other.first_array_size &&
           second_array_size == other.second_array_size;
  } else if (status == kFirstValueEmpty) {
    if (key != other.key) return false;
    if (type != other.type) return false;
    switch (type) { /*{{{*/
      case kInt32:
        return value.second_value_int32 == other.value.second_value_int32;
      case kUInt32:
        return value.second_value_uint32 == other.value.second_value_uint32;
      case kInt64:
        return value.second_value_int64 == other.value.second_value_int64;
      case kUInt64:
        return value.second_value_uint64 == other.value.second_value_uint64;
      case kDouble:
        return value.second_value_double == other.value.second_value_double;
      case kFloat:
        return value.second_value_float == other.value.second_value_float;
      case kBool:
        return value.second_value_bool == other.value.second_value_bool;
      case kEnum:
        return value.second_value_enum == other.value.second_value_enum;
      case kString:
        return value.second_value_string == other.value.second_value_string;
      case kObject:  // NOTE:htt, just return ture for object type
        return true;
      default:
        return false;
    } /*}}}*/
  } else if (status == kSecondValueEmpty) {
    if (key != other.key) return false;
    if (type != other.type) return false;
    switch (type) { /*{{{*/
      case kInt32:
        return value.first_value_int32 == other.value.first_value_int32;
      case kUInt32:
        return value.first_value_uint32 == other.value.first_value_uint32;
      case kInt64:
        return value.first_value_int64 == other.value.first_value_int64;
      case kUInt64:
        return value.first_value_uint64 == other.value.first_value_uint64;
      case kDouble:
        return value.first_value_double == other.value.first_value_double;
      case kFloat:
        return value.first_value_float == other.value.first_value_float;
      case kBool:
        return value.first_value_bool == other.value.first_value_bool;
      case kEnum:
        return value.first_value_enum == other.value.first_value_enum;
      case kString:
        return value.first_value_string == other.value.first_value_string;
      case kObject:  // NOTE:htt, just return ture for object type
        return true;
      default:
        return false;
    } /*}}}*/
  }

  return false;
} /*}}}*/

base::Code PBDiffWithOutExtension(const ::google::protobuf::Message &msg_first,
                                  const ::google::protobuf::Message &msg_second, bool *is_diff,
                                  std::deque<DiffContent> *diff_contents) { /*{{{*/
  if (is_diff == NULL) return base::kInvalidParam;

  *is_diff = false;
  if (diff_contents != NULL) {
    diff_contents->clear();
  }

  return PBDiffWithOutExtension(msg_first, msg_second, "", is_diff, diff_contents);
} /*}}}*/

base::Code PBDiffWithOutExtension(const ::google::protobuf::Message &msg_first,
                                  const ::google::protobuf::Message &msg_second,
                                  const std::string &key_prefix, bool *is_diff,
                                  std::deque<DiffContent> *diff_contents) { /*{{{*/
  if (is_diff == NULL) return base::kInvalidParam;

  if (msg_first.GetTypeName() != msg_second.GetTypeName()) { /*{{{*/
    *is_diff = true;
    DiffContent diff_content;
    diff_content.status = kPBTypeNotEqual;
    diff_content.first_type = msg_first.GetTypeName();
    diff_content.second_type = msg_second.GetTypeName();
    diff_contents->push_back(diff_content);
    return base::kOk;
  } /*}}}*/

  base::Code ret = base::kOk;

  const ::google::protobuf::Descriptor *descriptor = msg_first.GetDescriptor();
  const ::google::protobuf::Reflection *reflection = msg_first.GetReflection();
  if (descriptor == NULL || reflection == NULL) return base::kInvalidParam;

  for (int i = 0; i < (int)descriptor->field_count(); ++i) { /*{{{*/
    const ::google::protobuf::FieldDescriptor *field_desc = descriptor->field(i);
    if (field_desc == NULL) return base::kInvalidParam;

    DiffContent diff_content;
    const std::string &key = field_desc->name();
    if (key_prefix == "") {
      diff_content.key = key;
    } else {
      diff_content.key = key_prefix + "." + key;
    }

    if (field_desc->is_repeated()) {
      int first_field_size = reflection->FieldSize(msg_first, field_desc);
      int second_field_size = reflection->FieldSize(msg_second, field_desc);
      if (first_field_size == 0 && second_field_size == 0) {
        continue;  // NOTE:htt, same array
      }

      switch (field_desc->cpp_type()) { /*{{{*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32:
          DIFF_ARRAY_OPERATION(kInt32, int32, int, Int32)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
          DIFF_ARRAY_OPERATION(kUInt32, uint32, uint32_t, UInt32)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64:
          DIFF_ARRAY_OPERATION(kInt64, int64, int64_t, Int64)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
          DIFF_ARRAY_OPERATION(kUInt64, uint64, uint64_t, UInt64)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
          DIFF_ARRAY_OPERATION(kDouble, double, double, Double)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
          DIFF_ARRAY_OPERATION(kFloat, float, float, Float)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
          DIFF_ARRAY_OPERATION(kBool, bool, bool, Bool)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
          diff_content.type = kEnum;  // NOTE:htt, using int value for enum type
          if (first_field_size == second_field_size) {
            for (int j = 0; j < first_field_size; ++j) {
              const ::google::protobuf::EnumValueDescriptor *pb_first_enum =
                  reflection->GetRepeatedEnum(msg_first, field_desc, j);
              if (pb_first_enum == NULL) return base::kInvalidParam;
              int first_value = pb_first_enum->number();

              const ::google::protobuf::EnumValueDescriptor *pb_second_enum =
                  reflection->GetRepeatedEnum(msg_second, field_desc, j);
              if (pb_second_enum == NULL) return base::kInvalidParam;
              int second_value = pb_second_enum->number();

              if (first_value != second_value) {
                *is_diff = true;
                char tmp_buf[base::kSmallBufLen] = {0};
                snprintf(tmp_buf, sizeof(tmp_buf) - 1, ".%d", j);
                diff_content.key = diff_content.key + tmp_buf;
                diff_content.status = kValueNotEqual;
                diff_content.value.first_value_enum = first_value;
                diff_content.value.second_value_enum = second_value;

                diff_contents->push_back(diff_content);
                break;
              }
            }
          } else {
            *is_diff = true;
            diff_content.status = kArraySizeNotEqual;
            diff_content.first_array_size = first_field_size;
            diff_content.second_array_size = second_field_size;
            diff_contents->push_back(diff_content);
          }

          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING:
          DIFF_ARRAY_OPERATION(kString, string, std::string, String)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
          diff_content.type = kObject;

          if (first_field_size == second_field_size) {
            for (int j = 0; j < first_field_size; ++j) {
              const ::google::protobuf::Message &first_child_msg =
                  reflection->GetRepeatedMessage(msg_first, field_desc, j);

              const ::google::protobuf::Message &second_child_msg =
                  reflection->GetRepeatedMessage(msg_second, field_desc, j);

              char tmp_buf[base::kSmallBufLen] = {0};
              snprintf(tmp_buf, sizeof(tmp_buf) - 1, ".%d", j);
              ret = PBDiffWithOutExtension(first_child_msg, second_child_msg,
                                           diff_content.key + tmp_buf, is_diff, diff_contents);
              if (ret != base::kOk) return ret;
            }
          } else {
            *is_diff = true;
            diff_content.status = kArraySizeNotEqual;
            diff_content.first_array_size = first_field_size;
            diff_content.second_array_size = second_field_size;
            diff_contents->push_back(diff_content);
          }

          break;
        } /*}}}*/
        default:
          return base::kInvalidParam;
      }      /*}}}*/
    } else { /*{{{*/
      bool is_first_value_exist = false;
      ret = IsValueExist(msg_first, reflection, field_desc, &is_first_value_exist);
      if (ret != base::kOk) return ret;
      bool is_second_value_exist = false;
      ret = IsValueExist(msg_second, reflection, field_desc, &is_second_value_exist);
      if (ret != base::kOk) return ret;

      if (is_first_value_exist == false && is_second_value_exist == false) {
        continue;
      }

      switch (field_desc->cpp_type()) { /*{{{*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32:
          DIFF_OPERATION(kInt32, int32, Int32)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
          DIFF_OPERATION(kUInt32, uint32, UInt32)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64:
          DIFF_OPERATION(kInt64, int64, Int64)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
          DIFF_OPERATION(kUInt64, uint64, UInt64)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
          DIFF_OPERATION(kDouble, double, Double)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
          DIFF_OPERATION(kFloat, float, Float)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
          DIFF_OPERATION(kBool, bool, Bool)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
          diff_content.type = kEnum;  // NOTE:htt, using int value for enum type
          if (is_first_value_exist) {
            const ::google::protobuf::EnumValueDescriptor *pb_enum =
                reflection->GetEnum(msg_first, field_desc);
            if (pb_enum == NULL) return base::kInvalidParam;
            diff_content.value.first_value_enum = pb_enum->number();
          }

          if (is_second_value_exist) {
            const ::google::protobuf::EnumValueDescriptor *pb_enum =
                reflection->GetEnum(msg_second, field_desc);
            if (pb_enum == NULL) return base::kInvalidParam;
            diff_content.value.second_value_enum = pb_enum->number();
          }

          if (is_first_value_exist == is_second_value_exist) {
            if (diff_content.value.first_value_enum == diff_content.value.second_value_enum) {
              break;
            } else {
              *is_diff = true;
              diff_content.status = kValueNotEqual;
            }
          } else {
            *is_diff = true;
            if (is_first_value_exist == false) {
              diff_content.status = kFirstValueEmpty;
            } else {
              diff_content.status = kSecondValueEmpty;
            }
          }
          diff_contents->push_back(diff_content);

          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING:
          DIFF_OPERATION(kString, string, String)
        case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
          diff_content.type = kObject;

          if (is_first_value_exist == is_second_value_exist) {  // NOTE:htt, both exist
            const ::google::protobuf::Message &first_child_msg =
                reflection->GetMessage(msg_first, field_desc);
            const ::google::protobuf::Message &second_child_msg =
                reflection->GetMessage(msg_second, field_desc);
            ret = PBDiffWithOutExtension(first_child_msg, second_child_msg, diff_content.key,
                                         is_diff, diff_contents);
            if (ret != base::kOk) return ret;
          } else {
            *is_diff = true;
            if (is_first_value_exist == false) {
              diff_content.status = kFirstValueEmpty;
            } else {
              diff_content.status = kSecondValueEmpty;
            }
            diff_contents->push_back(diff_content);
          }

          break;
        } /*}}}*/
        default:
          return base::kInvalidParam;
      } /*}}}*/
    }   /*}}}*/
  }     /*}}}*/

  return ret;
} /*}}}*/

// NOTE: repeated fields are not considered
base::Code IsValueExist(const ::google::protobuf::Message &msg,
                        const ::google::protobuf::Reflection *reflection,
                        const ::google::protobuf::FieldDescriptor *field_desc,
                        bool *is_value_exist) { /*{{{*/
  if (reflection == NULL || field_desc == NULL) return base::kInvalidParam;

  if (field_desc->is_repeated()) return base::kInvalidParam;

  *is_value_exist = false;
  if (!field_desc->is_optional() || reflection->HasField(msg, field_desc)) {
    *is_value_exist = true;
  }

  return base::kOk;
} /*}}}*/

base::Code InitJsonValue(const std::string &json, const std::set<std::string> init_keys_list,
                         std::string *dst_str) { /*{{{*/
  if (dst_str == NULL) return base::kInvalidParam;

  rapidjson::Document d;
  d.Parse(json.c_str(), json.size());
  if (d.HasParseError()) return base::kParseJsonFailed;
  if (!d.IsObject()) return base::kInvalidParam;

  rapidjson::Document dst_doc;
  base::Code ret = InitJsonValue(d, init_keys_list, &dst_doc, dst_doc.GetAllocator());
  if (ret != base::kOk) return ret;

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  dst_doc.Accept(writer);

  dst_str->assign(buffer.GetString(), buffer.GetSize());
  return base::kOk;
} /*}}}*/

base::Code InitJsonValue(const rapidjson::Value &json, const std::set<std::string> init_keys_list,
                         rapidjson::Value *dst_json,
                         rapidjson::Value::AllocatorType &alloc) { /*{{{*/
  if (dst_json == NULL) return base::kInvalidParam;
  if (!json.IsObject()) return base::kInvalidParam;
  base::Code ret = base::kOk;

  dst_json->SetObject();

  rapidjson::Value::ConstMemberIterator mem_it = json.MemberBegin();
  for (; mem_it != json.MemberEnd(); ++mem_it) {
    if (!mem_it->name.IsString()) return base::kInvalidParam;
    std::string field_name(mem_it->name.GetString(), mem_it->name.GetStringLength());

    if (mem_it->value.IsArray()) {
      rapidjson::Value child_arr;
      child_arr.SetArray();
      if (mem_it->value.Size() == 0) { /*{{{*/
        dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                            child_arr.Move(), alloc);
        continue;
      } /*}}}*/

      if (init_keys_list.count(field_name) == 0) {  // NOTE:htt, no need to init

        if (mem_it->value[0].IsObject()) {
          rapidjson::Value::ConstValueIterator v_it = mem_it->value.Begin();
          for (; v_it != mem_it->value.End(); ++v_it) {
            rapidjson::Value v;
            ret = InitJsonValue(*v_it, init_keys_list, &v, alloc);
            if (ret != base::kOk) return ret;
            child_arr.PushBack(v.Move(), alloc);
          }
        } else {  // NOTE:htt, just copy value for plain type
          child_arr.CopyFrom(mem_it->value, alloc);
        }

        dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                            child_arr.Move(), alloc);
      } else {  // NOTE:htt, need to init
        if (mem_it->value[0].IsInt() || mem_it->value[0].IsUint() || mem_it->value[0].IsInt64() ||
            mem_it->value[0].IsUint64()) {
          child_arr.PushBack(rapidjson::Value(0).Move(), alloc);
        } else if (mem_it->value[0].IsDouble() || mem_it->value[0].IsFloat()) {
          child_arr.PushBack(rapidjson::Value(0.0).Move(), alloc);
        } else if (mem_it->value[0].IsBool()) {
          child_arr.PushBack(rapidjson::Value(false).Move(), alloc);
        } else if (mem_it->value[0].IsString()) {
          child_arr.PushBack(rapidjson::Value("", 0, alloc).Move(), alloc);
        } else if (mem_it->value[0].IsObject()) {
          // NOTE:htt, The operation is not allowed because it is not a leaf node; just continue
          rapidjson::Value::ConstValueIterator v_it = mem_it->value.Begin();
          for (; v_it != mem_it->value.End(); ++v_it) {
            rapidjson::Value v;
            ret = InitJsonValue(*v_it, init_keys_list, &v, alloc);
            if (ret != base::kOk) return ret;
            child_arr.PushBack(v.Move(), alloc);
          }
        }

        dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                            child_arr.Move(), alloc);
      }
    } else {                                        // NOTE:htt, not array
      if (init_keys_list.count(field_name) == 0) {  // NOTE:htt, no need to init
        rapidjson::Value v;

        if (mem_it->value.IsObject()) {
          ret = InitJsonValue(mem_it->value, init_keys_list, &v, alloc);
          if (ret != base::kOk) return ret;
        } else {  // NOTE:htt, just copy value for plain type
          v.CopyFrom(mem_it->value, alloc);
        }

        dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                            v.Move(), alloc);
      } else {  // NOTE:htt, need to init
        if (mem_it->value.IsInt() || mem_it->value.IsUint() || mem_it->value.IsInt64() ||
            mem_it->value.IsUint64()) {
          dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                              rapidjson::Value(0).Move(), alloc);
        } else if (mem_it->value.IsDouble() || mem_it->value.IsFloat()) {
          dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                              rapidjson::Value(0.0).Move(), alloc);
        } else if (mem_it->value.IsBool()) {
          dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                              rapidjson::Value(false).Move(), alloc);
        } else if (mem_it->value.IsString()) {
          dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                              rapidjson::Value("", 0, alloc).Move(), alloc);
        } else if (mem_it->value.IsObject()) {
          // NOTE:htt, The operation is not allowed because it is not a leaf node; just continue
          rapidjson::Value v;
          ret = InitJsonValue(mem_it->value, init_keys_list, &v, alloc);
          if (ret != base::kOk) return ret;
          dst_json->AddMember(rapidjson::Value(field_name.c_str(), field_name.size(), alloc).Move(),
                              v.Move(), alloc);
        }
      }
    }
  }

  return ret;
} /*}}}*/

base::Code IsJsonValueValidEncoding(const std::string &value, bool *is_valid) { /*{{{*/
  if (is_valid == NULL) return base::kInvalidParam;
  *is_valid = true;

  rapidjson::Document doc;
  std::string json_str = "{\"value\":\"" + value + "\" }";
  doc.Parse<rapidjson::kParseValidateEncodingFlag>(json_str.c_str());
  if (doc.HasParseError()) {
    if (doc.GetParseError() == rapidjson::kParseErrorStringUnicodeEscapeInvalidHex ||
        doc.GetParseError() == rapidjson::kParseErrorStringUnicodeSurrogateInvalid ||
        doc.GetParseError() == rapidjson::kParseErrorStringEscapeInvalid ||
        doc.GetParseError() == rapidjson::kParseErrorStringInvalidEncoding) {
      *is_valid = false;
      return base::kOk;
    }

    return base::kParseJsonFailed;
  }

  return base::kOk;
} /*}}}*/

base::Code GetNthLevelKeysOfJson(const std::string &json, uint32_t dest_level,
                                 std::vector<std::string> *keys) {/*{{{*/
  if (keys == NULL) return base::kInvalidParam;

  rapidjson::Document d;
  d.Parse(json.c_str(), json.size());
  if (d.HasParseError()) return base::kInvalidParam;
  if (!d.IsObject()) return base::kInvalidParam;

  return GetNthLevelKeysOfJson(d, 1, dest_level, keys);
}/*}}}*/

base::Code GetNthLevelKeysOfJson(const rapidjson::Value &json, uint32_t current_level,
                                 uint32_t dest_level, std::vector<std::string> *keys) { /*{{{*/
  if (keys == NULL) return base::kInvalidParam;
  if (!json.IsObject()) return base::kInvalidParam;

  if (current_level > dest_level) return base::kInvalidParam;

  base::Code ret = base::kOk;
  rapidjson::Value::ConstMemberIterator mem_it = json.MemberBegin();
  for (; mem_it != json.MemberEnd(); ++mem_it) {
    if (!mem_it->name.IsString()) return base::kInvalidParam;
    std::string field_name(mem_it->name.GetString(), mem_it->name.GetStringLength());

    if (current_level == dest_level) {
      keys->push_back(field_name);
      continue;
    }

    if (mem_it->value.IsObject()) {
      ret = GetNthLevelKeysOfJson(mem_it->value, current_level + 1, dest_level, keys);
      if (ret != base::kOk) return ret;
    } else if (mem_it->value.IsArray()) {
      rapidjson::Value::ConstValueIterator v_it = mem_it->value.Begin();
      for (; v_it != mem_it->value.End(); ++v_it) {
        if (v_it->IsObject()) {
          ret = GetNthLevelKeysOfJson(*v_it, current_level + 1, dest_level, keys);
          if (ret != base::kOk) return ret;
          continue;
        }
      }
    } else {
      // NOTE:htt, For plain objects, it is impossible to continue drilling down
      continue;
    }
  }

  return ret;
} /*}}}*/

}  // namespace proto
