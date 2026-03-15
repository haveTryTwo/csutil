// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sock/rpc_proto.h"

#include "base/coding.h"
#include "base/common.h"
#include "base/msg.h"
#include "base/util.h"

namespace base {

/**
 * @brief 从 TCP 流中判断一个完整包的实际长度
 *
 * 协议格式: [Head(4B)][Magic(4B)][FrameCode(4B)][UserData(NB)]
 * Head 中存储的 len = kExtHeadLen + N，即 Magic + FrameCode + UserData 的总长度
 *
 * @param src_data 接收缓冲区起始地址
 * @param src_data_len 当前缓冲区中的数据长度
 * @param real_len 输出参数，完整包的总长度（Head + len）
 * @return kOk 成功；kDataNotEnough 数据不足；kInvalidParam 参数无效；kInvalidSize 长度非法
 */
Code DefaultProtoFunc(const char *src_data, int src_data_len, int *real_len) { /*{{{*/
  if (src_data == NULL || src_data_len < 0 || real_len == NULL) return kInvalidParam;

  if (src_data_len < (kHeadLen + kExtHeadLen)) return kDataNotEnough;

  uint32_t len = 0;
  Code r = DecodeFixed32(std::string(src_data, kHeadLen), &len);
  if (r != kOk) return r;
  if (len >= (uint32_t)(kIntMax - kHeadLen)) return kInvalidSize;
  if (len < (uint32_t)kExtHeadLen) return kInvalidSize;

  if (src_data_len < (int)(kHeadLen + len)) return kDataNotEnough;

  *real_len = kHeadLen + len;

  return kOk;
} /*}}}*/

/**
 * @brief 从完整包中提取用户数据
 *
 * 协议格式: [Head(4B)][Magic(4B)][FrameCode(4B)][UserData(NB)]
 * 1. 校验 Magic 是否匹配 kProtoMagic
 * 2. 解码 FrameCode，如果不为 0，返回对应的框架错误码
 * 3. 提取 UserData 部分赋值给 user_data
 *
 * @param src_data 完整包的起始地址
 * @param src_data_len 完整包的长度
 * @param user_data 输出参数，提取出的用户业务数据
 * @return kOk 成功；kFrameInvalidMagic Magic 校验失败；框架错误码(700-799) FrameCode 非零
 */
Code DefaultGetUserDataFunc(const char *src_data, int src_data_len, std::string *user_data) { /*{{{*/
  if (src_data == NULL || src_data_len < 0 || user_data == NULL) return kInvalidParam;

  if (src_data_len < (kHeadLen + kExtHeadLen)) return kInvalidData;

  // 解码 Head，获取 payload 长度
  uint32_t len = 0;
  Code r = DecodeFixed32(std::string(src_data, kHeadLen), &len);
  if (r != kOk) return r;
  if (len >= (uint32_t)(kIntMax - kHeadLen)) return kInvalidSize;
  if (len < (uint32_t)kExtHeadLen) return kInvalidData;

  if (src_data_len != (int)(kHeadLen + len)) return kInvalidData;

  // 校验 Magic
  uint32_t magic = 0;
  r = DecodeFixed32(std::string(src_data + kHeadLen, kMagicLen), &magic);
  if (r != kOk) return r;
  if (magic != kProtoMagic) return kFrameInvalidMagic;

  // 解码 FrameCode
  uint32_t frame_code = 0;
  r = DecodeFixed32(std::string(src_data + kHeadLen + kMagicLen, kFrameCodeLen), &frame_code);
  if (r != kOk) return r;

  // 如果 FrameCode 不为 0，返回框架错误码
  if (frame_code != 0) {
    return (Code)frame_code;
  }

  // 提取 UserData
  uint32_t user_data_len = len - kExtHeadLen;
  user_data->assign(src_data + kHeadLen + kExtHeadLen, user_data_len);

  return kOk;
} /*}}}*/

/**
 * @brief 将用户数据按协议格式封装为完整的发送数据
 *
 * 协议格式: [Head(4B)][Magic(4B)][FrameCode(4B)][UserData(NB)]
 * Head = kExtHeadLen + user_data.size()
 * Magic = kProtoMagic
 * FrameCode = 0（正常）
 *
 * @param user_data 用户业务数据
 * @param real_data 输出参数，编码后的完整协议数据
 * @return kOk 成功；kInvalidParam 参数无效
 */
Code DefaultFormatUserDataFunc(const std::string user_data, std::string *real_data) { /*{{{*/
  if (real_data == NULL) return kInvalidParam;

  // Head: payload 长度 = Magic(4) + FrameCode(4) + UserData(N)
  Code r = EncodeFixed32(kExtHeadLen + user_data.size(), real_data);
  if (r != kOk) return r;

  // Magic
  r = EncodeFixed32(kProtoMagic, real_data);
  if (r != kOk) return r;

  // FrameCode = 0 (正常)
  r = EncodeFixed32(0, real_data);
  if (r != kOk) return r;

  // UserData
  real_data->append(user_data);

  return kOk;
} /*}}}*/

/**
 * @brief 判断错误码是否属于框架错误码范围 [700, 799]
 * @param frame_code 待判断的错误码
 * @return true 属于框架错误码；false 不属于
 */
bool IsFrameError(uint32_t frame_code) { /*{{{*/
  return frame_code >= kFrameErrMin && frame_code <= kFrameErrMax;
} /*}}}*/

/**
 * @brief 封装框架错误响应，仅包含 Head + Magic + FrameCode，不包含 UserData
 *
 * 协议格式: [Head(4B)][Magic(4B)][FrameCode(4B)]
 * Head = kExtHeadLen (Magic + FrameCode 的总长度，即 8)
 *
 * @param frame_code 框架错误码，取值范围 [700, 799]
 * @param real_data 输出参数，编码后的完整响应数据
 * @return kOk 成功；kInvalidParam 参数无效或错误码不在范围内
 */
Code FormatFrameErrorResp(uint32_t frame_code, std::string *real_data) { /*{{{*/
  if (real_data == NULL) return kInvalidParam;
  if (!IsFrameError(frame_code)) return kInvalidParam;

  // Head: payload 长度 = Magic(4) + FrameCode(4) = kExtHeadLen
  Code r = EncodeFixed32(kExtHeadLen, real_data);
  if (r != kOk) return r;

  // Magic
  r = EncodeFixed32(kProtoMagic, real_data);
  if (r != kOk) return r;

  // FrameCode
  r = EncodeFixed32(frame_code, real_data);
  if (r != kOk) return r;

  return kOk;
} /*}}}*/

}  // namespace base
