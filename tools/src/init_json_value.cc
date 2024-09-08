// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>
#include <set>
#include <vector>

#include <assert.h>
#include <stdio.h>

#include "base/common.h"
#include "base/file_util.h"
#include "base/util.h"
#include "proto/pb_util.h"

#include "init_json_value.h"

#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "pb_src/base.pb.h"

namespace tools {

base::Code InitJsonValue(const std::string &src_file, const std::string &dst_file,
                         const std::string &init_keys) { /*{{{*/
  if (src_file.empty() || dst_file.empty() || init_keys.empty()) return base::kInvalidParam;

  FILE *src_fp = fopen(src_file.c_str(), "r");
  if (src_fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", src_file.c_str());
    return base::kOpenFileFailed;
  }
  FILE *dst_fp = fopen(dst_file.c_str(), "w+");
  if (dst_fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", dst_file.c_str());
    return base::kOpenFileFailed;
  }

  base::Code ret = base::kOk;
  std::set<std::string> init_keys_list;
  ret = base::Strtok(init_keys, ',', &init_keys_list);
  if (ret != base::kOk) {
    fprintf(stderr, "Failed to strtok:%s, ret:%d\n", init_keys.c_str(), ret);
    return ret;
  }

  fprintf(stderr, "[BEGIN] Start to read file:%s\n", src_file.c_str());

  uint32_t line = 0;
  while (true) { /*{{{*/
    ++line;
    std::string tmp_cnt;
    ret = base::PumpStringData(&tmp_cnt, src_fp);
    if (ret != base::kOk) { /*{{{*/
      if (ret == base::kFileIsEnd) {
        --line;
        fprintf(stderr, "[END] File:%s has been finished! line:%u\n", src_file.c_str(), line);
        ret = base::kOk;
        break;
      } else {
        fprintf(stderr, "[FAILED] Reading file:%s failed!\n", src_file.c_str());
        break;
      }
    } /*}}}*/

    std::string right_delims = " \t\r\n";
    std::string out_str;
    ret = base::TrimRight(tmp_cnt, right_delims, &out_str);
    assert(ret == base::kOk);
    if (out_str.size() == 0) continue;

    std::string dst_json;
    ret = proto::InitJsonValue(out_str, init_keys_list, &dst_json);
    if (ret != base::kOk) {
      fprintf(stderr, "[FAILED] init json:%s failed! ret:%d\n", out_str.c_str(), ret);
      break;
    }

    ret = base::DumpStringData(dst_json + "\n", dst_fp);
    if (ret != base::kOk) {
      fprintf(stderr, "[FAILED] Writing file:%s failed!\n", dst_file.c_str());
      break;
    }
  } /*}}}*/

  fclose(src_fp);
  src_fp = NULL;
  fclose(dst_fp);
  dst_fp = NULL;

  return ret;
} /*}}}*/

base::Code InitJsonValue(const std::string &src_cnt, const std::string &init_keys, std::string *dst_cnt) { /*{{{*/
  if (dst_cnt == NULL || init_keys.empty()) return base::kInvalidParam;

  dst_cnt->clear();
  if (src_cnt.empty()) return base::kOk;

  base::Code ret = base::kOk;
  std::set<std::string> init_keys_list;
  ret = base::Strtok(init_keys, ',', &init_keys_list);
  if (ret != base::kOk) {
    fprintf(stderr, "Failed to strtok:%s, ret:%d\n", init_keys.c_str(), ret);
    return ret;
  }

  ret = proto::InitJsonValue(src_cnt, init_keys_list, dst_cnt);
  if (ret != base::kOk) {
    fprintf(stderr, "[FAILED] init json:%s failed! ret:%d\n", src_cnt.c_str(), ret);
    return ret;
  }

  return ret;
} /*}}}*/

base::Code SerializePBForJsonContent(const std::string &src_file, const std::string &dst_file, int level) { /*{{{*/
  if (src_file.empty() || dst_file.empty() || level < 1) return base::kInvalidParam;

  FILE *src_fp = fopen(src_file.c_str(), "r");
  if (src_fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", src_file.c_str());
    return base::kOpenFileFailed;
  }
  FILE *dst_fp = fopen(dst_file.c_str(), "w+");
  if (dst_fp == NULL) {
    fprintf(stderr, "Failed to open path:%s\n", dst_file.c_str());
    return base::kOpenFileFailed;
  }

  fprintf(stderr, "[BEGIN] Start to read file:%s\n", src_file.c_str());

  base::Code ret = base::kOk;
  uint32_t line = 0;
  std::string cnt;
  while (true) { /*{{{*/
    ++line;
    std::string tmp_cnt;
    ret = base::PumpStringData(&tmp_cnt, src_fp);
    if (ret != base::kOk) { /*{{{*/
      if (ret == base::kFileIsEnd) {
        --line;
        fprintf(stderr, "[END] File:%s has been finished! line:%u\n", src_file.c_str(), line);
        ret = base::kOk;
        break;
      } else {
        fprintf(stderr, "[FAILED] Reading file:%s failed!\n", src_file.c_str());
        break;
      }
    } /*}}}*/

    std::string right_delims = " \t\r\n";
    std::string out_str;
    ret = base::TrimRight(tmp_cnt, right_delims, &out_str);
    assert(ret == base::kOk);
    if (out_str.size() == 0) continue;
    cnt.append(out_str);
  } /*}}}*/

  if (ret == base::kOk) {
    rapidjson::Document doc;
    doc.Parse(cnt.c_str());
    if (doc.HasParseError()) {
      fprintf(stderr, "[FAILED] to parse!, ret:%d, str:%s\n", doc.GetParseError(), cnt.c_str());
      ret = base::kParseJsonFailed;
      goto finished;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::string tmp_str(buffer.GetString(), buffer.GetSize());
    for (int i = 0; i < level; ++i) {
      base::DataBase data_base;
      data_base.set_msg(tmp_str);
      tmp_str = data_base.DebugString();
    }
    ret = base::DumpStringData(tmp_str + "\n", dst_fp);
    if (ret != base::kOk) {
      fprintf(stderr, "[FAILED] Writing file:%s failed!\n", dst_file.c_str());
      goto finished;
    }
  }

finished:
  fclose(src_fp);
  src_fp = NULL;
  fclose(dst_fp);
  dst_fp = NULL;

  return ret;
} /*}}}*/
}  // namespace tools
