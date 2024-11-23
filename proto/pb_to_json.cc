// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "proto/pb_to_json.h"

namespace proto {

// base::Code PBToJsonWithOutExtension(const ::google::protobuf::Message &msg, std::string *json)
// {/*{{{*/
//     if (json == NULL) return base::kInvalidParam;
//     // json->clear();
//
//     const ::google::protobuf::Descriptor *descriptor = msg.GetDescriptor();
//     const ::google::protobuf::Reflection *reflection = msg.GetReflection();
//
//     // std::vector<const ::google::protobuf::FieldDescriptor*> fields;
//     // reflection->ListFields(msg, &fields);
//
//     bool first = true;
//     json->append("{");
//     for (int i = 0; i < (int)descriptor->field_count(); ++i)
//     {/*{{{*/
//         const ::google::protobuf::FieldDescriptor *field_desc = descriptor->field(i);
//         if (field_desc == NULL) return base::kInvalidParam;
//
//         // fprintf(stderr, "filed name:%s, full name:%s, repeated:%d\n",
//         field_desc->name().c_str(), field_desc->full_name().c_str(), field_desc->is_repeated());
//
//         if (field_desc->is_repeated())
//         {/*{{{*/
//             int field_size = reflection->FieldSize(msg, field_desc);
//             if (field_size == 0)
//             {
//                 continue; // Note: no need to record
//             }
//
//             if (first)
//             {
//                 first = false;
//             }
//             else
//             {
//                 json->append(",");
//             }
//
//             char buf[64] = {0};
//             // const std::string &key = field_desc->is_extension() ?
//             (field_desc->full_name()):(field_desc->name()); const std::string &key =
//             field_desc->name(); json->append("\"").append(key).append("\":[");
//
//             for (int j = 0; j < field_size; ++j)
//             {/*{{{*/
//                 if (j > 0)
//                 {
//                     json->append(",");
//                 }
//
//                 switch (field_desc->cpp_type())
//                 {
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32:
//                         {/*{{{*/
//                             int value = reflection->GetRepeatedInt32(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%d", value);
//
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
//                         {/*{{{*/
//                             uint32_t value = reflection->GetRepeatedUInt32(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%u", value);
//
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64:
//                         {/*{{{*/
//                             int64_t value = reflection->GetRepeatedInt64(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%lld", value);
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
//                         {/*{{{*/
//                             uint64_t value = reflection->GetRepeatedUInt64(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%llu", value);
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
//                         {/*{{{*/
//                             double value = reflection->GetRepeatedDouble(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%.*f", DBL_DIG, value);
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
//                         {/*{{{*/
//                             float value = reflection->GetRepeatedFloat(msg, field_desc, j);
//                             snprintf(buf, sizeof(buf), "%.*f", FLT_DIG, value);
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
//                         {/*{{{*/
//                             bool value = reflection->GetRepeatedBool(msg, field_desc, j);
//                             if (value)
//                                 snprintf(buf, sizeof(buf), "true");
//                             else
//                                 snprintf(buf, sizeof(buf), "false");
//
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
//                         {/*{{{*/
//                             const ::google::protobuf::EnumValueDescriptor *pb_enum =
//                             reflection->GetRepeatedEnum(msg, field_desc, j);
//                             //
//                             json->append("\"").append(key).append("\":").append(pb_enum->name());;
//                             snprintf(buf, sizeof(buf), "%u", pb_enum->number());
//                             json->append(buf);;
//                             break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING:
//                         {/*{{{*/
//                             json->append("\"").append(reflection->GetRepeatedString(msg,
//                             field_desc, j)).append("\""); break;
//                         }/*}}}*/
//                     case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
//                         {/*{{{*/
//                             const ::google::protobuf::Message& child_msg =
//                             reflection->GetRepeatedMessage(msg, field_desc, j);
//                             PBToJsonWithOutExtension(child_msg, json);
//                             break;
//                         }/*}}}*/
//                     default:
//                         fprintf(stderr, "Unkown cpp_type:%d\n", field_desc->cpp_type());
//                         return base::kInvalidParam;
//                 }
//             }/*}}}*/
//             json->append("]");
//         }/*}}}*/
//         else if (!field_desc->is_optional() || reflection->HasField(msg, field_desc))
//         {/*{{{*/
//             if (first)
//             {
//                 first = false;
//             }
//             else
//             {
//                 json->append(",");
//             }
//
//             char buf[64] = {0};
//             // const std::string &key = field_desc->is_extension() ?
//             (field_desc->full_name()):(field_desc->name()); const std::string &key =
//             field_desc->name(); switch (field_desc->cpp_type())
//             {
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32:
//                     {/*{{{*/
//                         int value = reflection->GetInt32(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%d", value);
//
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
//                     {/*{{{*/
//                         uint32_t value = reflection->GetUInt32(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%u", value);
//
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64:
//                     {/*{{{*/
//                         int64_t value = reflection->GetInt64(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%lld", value);
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
//                     {/*{{{*/
//                         uint64_t value = reflection->GetUInt64(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%llu", value);
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
//                     {/*{{{*/
//                         double value = reflection->GetDouble(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%.*f", DBL_DIG, value);
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
//                     {/*{{{*/
//                         float value = reflection->GetFloat(msg, field_desc);
//                         snprintf(buf, sizeof(buf), "%.*f", FLT_DIG, value);
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
//                     {/*{{{*/
//                         bool value = reflection->GetBool(msg, field_desc);
//                         if (value)
//                             snprintf(buf, sizeof(buf), "true");
//                         else
//                             snprintf(buf, sizeof(buf), "false");
//
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
//                     {/*{{{*/
//                         const ::google::protobuf::EnumValueDescriptor *pb_enum =
//                         reflection->GetEnum(msg, field_desc);
//                         // json->append("\"").append(key).append("\":").append(pb_enum->name());;
//                         snprintf(buf, sizeof(buf), "%u", pb_enum->number());
//                         json->append("\"").append(key).append("\":").append(buf);;
//                         break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING:
//                     {/*{{{*/
//                         json->append("\"").append(key).append("\":");;
//                         json->append("\"").append(reflection->GetString(msg,
//                         field_desc)).append("\""); break;
//                     }/*}}}*/
//                 case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
//                     {/*{{{*/
//                         json->append("\"").append(key).append("\":");;
//                         const ::google::protobuf::Message& child_msg =
//                         reflection->GetMessage(msg, field_desc);
//                         PBToJsonWithOutExtension(child_msg, json);
//                         break;
//                     }/*}}}*/
//                 default:
//                     fprintf(stderr, "Unkown cpp_type:%d\n", field_desc->cpp_type());
//                     return base::kInvalidParam;
//             }
//         }/*}}}*/
//     }/*}}}*/
//
//     json->append("}");
//
//     return base::kOk;
// }/*}}}*/

base::Code PBToJsonWithOutExtension(const ::google::protobuf::Message &msg, std::string *json) { /*{{{*/
  if (json == NULL) return base::kInvalidParam;

  rapidjson::Document doc;
  base::Code ret = PBToJsonWithOutExtension(msg, doc.GetAllocator(), &doc);
  if (ret != base::kOk) return ret;

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  json->assign(buffer.GetString(), buffer.GetSize());
  return base::kOk;
} /*}}}*/

base::Code JsonToPBWithOutExtension(const std::string &json, ::google::protobuf::Message *msg) { /*{{{*/
  if (msg == NULL) return base::kInvalidParam;

  rapidjson::Document d;
  d.Parse(json.c_str(), json.size());
  if (d.HasParseError()) return base::kInvalidParam;
  if (!d.IsObject()) return base::kInvalidParam;

  return JsonToPBWithOutExtension(d, msg);
} /*}}}*/

base::Code PBToJsonWithOutExtension(const ::google::protobuf::Message &msg, rapidjson::Value::AllocatorType &alloc,
                                    rapidjson::Value *json) { /*{{{*/
  if (json == NULL) return base::kInvalidParam;

  const ::google::protobuf::Descriptor *descriptor = msg.GetDescriptor();
  const ::google::protobuf::Reflection *reflection = msg.GetReflection();
  if (descriptor == NULL || reflection == NULL) return base::kInvalidParam;
  base::Code ret = base::kOk;

  json->SetObject();

  for (int i = 0; i < (int)descriptor->field_count(); ++i) { /*{{{*/
    const ::google::protobuf::FieldDescriptor *field_desc = descriptor->field(i);
    if (field_desc == NULL) return base::kInvalidParam;

    // fprintf(stderr, "filed_count:%d, filed name:%s, full name:%s, repeated:%d, ",
    // descriptor->field_count(), field_desc->name().c_str(), field_desc->full_name().c_str(),
    // field_desc->is_repeated()); if (!field_desc->is_repeated())
    // {
    //     fprintf(stderr, "has_field:%d\n", reflection->HasField(msg, field_desc));
    // }

    if (field_desc->is_repeated()) { /*{{{*/
      int field_size = reflection->FieldSize(msg, field_desc);
      if (field_size == 0) {
        continue;  // Note: no need to record
      }

      const std::string &key = field_desc->name();
      rapidjson::Value child_arr;
      child_arr.SetArray();

      for (int j = 0; j < field_size; ++j) { /*{{{*/
        switch (field_desc->cpp_type()) {
          case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32: { /*{{{*/
            int value = reflection->GetRepeatedInt32(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32: { /*{{{*/
            uint32_t value = reflection->GetRepeatedUInt32(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64: { /*{{{*/
            int64_t value = reflection->GetRepeatedInt64(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64: { /*{{{*/
            uint64_t value = reflection->GetRepeatedUInt64(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: { /*{{{*/
            double value = reflection->GetRepeatedDouble(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: { /*{{{*/
            float value = reflection->GetRepeatedFloat(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL: { /*{{{*/
            bool value = reflection->GetRepeatedBool(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
            const ::google::protobuf::EnumValueDescriptor *pb_enum = reflection->GetRepeatedEnum(msg, field_desc, j);
            if (pb_enum == NULL) return base::kInvalidParam;
            child_arr.PushBack(rapidjson::Value(pb_enum->number()).Move(),
                               alloc);  // Note: use number of enum
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING: { /*{{{*/
            std::string value = reflection->GetRepeatedString(msg, field_desc, j);
            child_arr.PushBack(rapidjson::Value(value.c_str(), value.size(), alloc).Move(), alloc);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
            const ::google::protobuf::Message &child_msg = reflection->GetRepeatedMessage(msg, field_desc, j);
            rapidjson::Value child_value;
            ret = PBToJsonWithOutExtension(child_msg, alloc, &child_value);
            if (ret != base::kOk) return ret;

            child_arr.PushBack(child_value.Move(), alloc);
            break;
          } /*}}}*/
          default:
            return base::kInvalidParam;
        }
      } /*}}}*/

      json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), child_arr.Move(), alloc);
    } /*}}}*/
    else if (!field_desc->is_optional() || reflection->HasField(msg, field_desc)) { /*{{{*/
      const std::string &key = field_desc->name();
      switch (field_desc->cpp_type()) {
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32: { /*{{{*/
          int value = reflection->GetInt32(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32: { /*{{{*/
          uint32_t value = reflection->GetUInt32(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64: { /*{{{*/
          int64_t value = reflection->GetInt64(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64: { /*{{{*/
          uint64_t value = reflection->GetUInt64(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: { /*{{{*/
          double value = reflection->GetDouble(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: { /*{{{*/
          float value = reflection->GetFloat(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL: { /*{{{*/
          bool value = reflection->GetBool(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), rapidjson::Value(value).Move(),
                          alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
          const ::google::protobuf::EnumValueDescriptor *pb_enum = reflection->GetEnum(msg, field_desc);
          if (pb_enum == NULL) return base::kInvalidParam;
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(),
                          rapidjson::Value(pb_enum->number()).Move(),
                          alloc);  // Note: use number of enum instead of string
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING: { /*{{{*/
          std::string value = reflection->GetString(msg, field_desc);
          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(),
                          rapidjson::Value(value.c_str(), value.size(), alloc).Move(), alloc);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
          const ::google::protobuf::Message &child_msg = reflection->GetMessage(msg, field_desc);
          rapidjson::Value child_value;
          ret = PBToJsonWithOutExtension(child_msg, alloc, &child_value);
          if (ret != base::kOk) return ret;

          json->AddMember(rapidjson::Value(key.c_str(), key.size(), alloc).Move(), child_value.Move(), alloc);
          break;
        } /*}}}*/
        default:
          return base::kInvalidParam;
      }
    } /*}}}*/
  } /*}}}*/

  return ret;
} /*}}}*/

base::Code JsonToPBWithOutExtension(const rapidjson::Value &json, ::google::protobuf::Message *msg) { /*{{{*/
  if (msg == NULL) return base::kInvalidParam;

  if (!json.IsObject()) return base::kInvalidParam;

  const ::google::protobuf::Descriptor *descriptor = msg->GetDescriptor();
  const ::google::protobuf::Reflection *reflection = msg->GetReflection();
  if (descriptor == NULL || reflection == NULL) return base::kInvalidParam;
  base::Code ret = base::kOk;

  rapidjson::Value::ConstMemberIterator mem_it = json.MemberBegin();
  for (; mem_it != json.MemberEnd(); ++mem_it) {
    if (!mem_it->name.IsString()) return base::kInvalidParam;
    std::string field_name(mem_it->name.GetString(), mem_it->name.GetStringLength());
    // const google::protobuf::FieldDescriptor *field_desc =
    // descriptor->FindFieldByLowercaseName(field_name);
    const google::protobuf::FieldDescriptor *field_desc = descriptor->FindFieldByName(field_name);
    if (field_desc == NULL) return base::kInvalidParam;

    if (field_desc->is_repeated()) { /*{{{*/
      if (!mem_it->value.IsArray()) return base::kInvalidParam;

      rapidjson::Value::ConstValueIterator v_it = mem_it->value.Begin();
      for (; v_it != mem_it->value.End(); ++v_it) { /*{{{*/
        switch (field_desc->cpp_type()) {
          case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32: { /*{{{*/
            if (!v_it->IsInt()) return base::kInvalidParam;
            int value = v_it->GetInt();
            msg->GetReflection()->AddInt32(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32: { /*{{{*/
            if (!v_it->IsUint()) return base::kInvalidParam;
            uint32_t value = v_it->GetUint();
            msg->GetReflection()->AddUInt32(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64: { /*{{{*/
            if (!v_it->IsInt64()) return base::kInvalidParam;
            int64_t value = v_it->GetInt64();
            msg->GetReflection()->AddInt64(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64: { /*{{{*/
            if (!v_it->IsUint64()) return base::kInvalidParam;
            uint64_t value = v_it->GetUint64();
            msg->GetReflection()->AddUInt64(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: { /*{{{*/
            if (!v_it->IsDouble()) return base::kInvalidParam;
            double value = v_it->GetDouble();
            msg->GetReflection()->AddDouble(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: { /*{{{*/
            if (!v_it->IsFloat()) return base::kInvalidParam;
            float value = v_it->GetFloat();
            msg->GetReflection()->AddFloat(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL: { /*{{{*/
            if (!v_it->IsBool()) return base::kInvalidParam;
            bool value = v_it->GetBool();
            msg->GetReflection()->AddBool(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
            if (!v_it->IsNumber() && !v_it->IsString()) return base::kInvalidParam;

            const ::google::protobuf::EnumDescriptor *desc = field_desc->enum_type();
            if (desc == NULL) return base::kInvalidParam;
            const ::google::protobuf::EnumValueDescriptor *enum_value = NULL;

            if (v_it->IsNumber()) {
              int value = 0;
              if (v_it->IsInt())
                value = v_it->GetInt();
              else if (v_it->IsUint())
                value = v_it->GetUint();
              else if (v_it->IsInt64())
                value = v_it->GetInt64();
              else if (v_it->IsUint64())
                value = v_it->GetUint64();

              enum_value = desc->FindValueByNumber(value);
            } else {
              std::string value(v_it->GetString(), v_it->GetStringLength());

              enum_value = desc->FindValueByName(value);
            }

            if (enum_value == NULL) return base::kInvalidParam;
            msg->GetReflection()->AddEnum(msg, field_desc, enum_value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING: { /*{{{*/
            if (!v_it->IsString()) return base::kInvalidParam;
            std::string value(v_it->GetString(), v_it->GetStringLength());
            msg->GetReflection()->AddString(msg, field_desc, value);
            break;
          } /*}}}*/
          case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
            if (!v_it->IsObject()) return base::kInvalidParam;
            ::google::protobuf::Message *child_msg = msg->GetReflection()->AddMessage(msg, field_desc);
            ret = JsonToPBWithOutExtension(*v_it, child_msg);
            if (ret != base::kOk) return ret;
            break;
          } /*}}}*/
          default:
            return base::kInvalidParam;
        }
      } /*}}}*/
    } /*}}}*/
    else { /*{{{*/
      switch (field_desc->cpp_type()) {
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT32: { /*{{{*/
          if (!mem_it->value.IsInt()) return base::kInvalidParam;
          int value = mem_it->value.GetInt();
          msg->GetReflection()->SetInt32(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT32: { /*{{{*/
          if (!mem_it->value.IsUint()) return base::kInvalidParam;
          uint32_t value = mem_it->value.GetUint();
          msg->GetReflection()->SetUInt32(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_INT64: { /*{{{*/
          if (!mem_it->value.IsInt64()) return base::kInvalidParam;
          int64_t value = mem_it->value.GetInt64();
          msg->GetReflection()->SetInt64(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_UINT64: { /*{{{*/
          if (!mem_it->value.IsUint64()) return base::kInvalidParam;
          uint64_t value = mem_it->value.GetUint64();
          msg->GetReflection()->SetUInt64(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: { /*{{{*/
          if (!mem_it->value.IsDouble()) return base::kInvalidParam;
          double value = mem_it->value.GetDouble();
          msg->GetReflection()->SetDouble(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: { /*{{{*/
          if (!mem_it->value.IsFloat()) return base::kInvalidParam;
          float value = mem_it->value.GetFloat();
          msg->GetReflection()->SetFloat(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_BOOL: { /*{{{*/
          if (!mem_it->value.IsBool()) return base::kInvalidParam;
          bool value = mem_it->value.GetBool();
          msg->GetReflection()->SetBool(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_ENUM: { /*{{{*/
          if (!mem_it->value.IsNumber() && !mem_it->value.IsString()) return base::kInvalidParam;

          const ::google::protobuf::EnumDescriptor *desc = field_desc->enum_type();
          if (desc == NULL) return base::kInvalidParam;
          const ::google::protobuf::EnumValueDescriptor *enum_value = NULL;

          if (mem_it->value.IsNumber()) {
            int value = 0;
            if (mem_it->value.IsInt())
              value = mem_it->value.GetInt();
            else if (mem_it->value.IsUint())
              value = mem_it->value.GetUint();
            else if (mem_it->value.IsInt64())
              value = mem_it->value.GetInt64();
            else if (mem_it->value.IsUint64())  // Note: shoule less than 2^32
              value = mem_it->value.GetUint64();

            enum_value = desc->FindValueByNumber(value);
          } else {
            std::string value(mem_it->value.GetString(), mem_it->value.GetStringLength());

            enum_value = desc->FindValueByName(value);
          }

          if (enum_value == NULL) return base::kInvalidParam;
          msg->GetReflection()->SetEnum(msg, field_desc, enum_value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_STRING: { /*{{{*/
          if (!mem_it->value.IsString()) return base::kInvalidParam;
          std::string value(mem_it->value.GetString(), mem_it->value.GetStringLength());
          msg->GetReflection()->SetString(msg, field_desc, value);
          break;
        } /*}}}*/
        case ::google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: { /*{{{*/
          if (!mem_it->value.IsObject()) return base::kInvalidParam;
          ::google::protobuf::Message *child_msg = msg->GetReflection()->MutableMessage(msg, field_desc);
          ret = JsonToPBWithOutExtension(mem_it->value, child_msg);
          if (ret != base::kOk) return ret;
          break;
        } /*}}}*/
        default:
          return base::kInvalidParam;
      }
    } /*}}}*/
  }

  return ret;
} /*}}}*/

}  // namespace proto
