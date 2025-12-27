// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/utf8.h"
#include <cstddef>
#include <cstdint>

namespace base {

namespace {

// 验证UTF-8后续字节是否有效 (必须是10xxxxxx格式)
inline bool IsValidContinuationByte(unsigned char byte) { return (byte & 0xC0) == 0x80; }

// 验证码点是否在有效范围内
inline bool IsValidCodepoint(uint32_t codepoint, int num_bytes) {
  if (num_bytes == 2) {
    return codepoint >= 0x80 && codepoint <= 0x7FF;
  } else if (num_bytes == 3) {
    // 排除代理对范围 (U+D800-U+DFFF)
    return codepoint >= 0x800 && codepoint <= 0xFFFF && (codepoint < 0xD800 || codepoint > 0xDFFF);
  } else if (num_bytes == 4) {
    return codepoint >= 0x10000 && codepoint <= 0x10FFFF;
  }
  return false;
}

}  // namespace

/**
 * @brief 检查字符串是否为有效的UTF-8编码
 *
 * 该函数验证输入字符串是否符合UTF-8编码规范，包括:
 * 1. 字节序列格式正确
 * 2. 码点在有效范围内
 * 3. 不包含过长编码
 * 4. 不包含代理对 (U+D800-U+DFFF)
 *
 * @param str 待检查的字符串
 * @return true 如果字符串是有效的UTF-8编码
 * @return false 如果字符串不是有效的UTF-8编码
 */
bool IsUtf8(const std::string& str) {
  const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
  const size_t len = str.length();
  size_t i = 0;

  while (i < len) {
    const unsigned char byte = bytes[i];

    // ASCII字符 (0x00-0x7F): 单字节
    if (byte <= 0x7F) {
      i++;
      continue;
    }

    // 多字节UTF-8序列
    int num_bytes = 0;
    uint32_t codepoint = 0;

    // 确定字节数和起始码点值
    if ((byte & 0xE0) == 0xC0) {
      num_bytes = 2;
      codepoint = byte & 0x1F;
    } else if ((byte & 0xF0) == 0xE0) {
      num_bytes = 3;
      codepoint = byte & 0x0F;
    } else if ((byte & 0xF8) == 0xF0) {
      num_bytes = 4;
      codepoint = byte & 0x07;
    } else {
      return false;
    }

    // 检查是否有足够的字节
    if (i + num_bytes > len) {
      return false;
    }

    // 处理后续字节 (10xxxxxx)
    for (int j = 1; j < num_bytes; ++j) {
      const unsigned char continuation_byte = bytes[i + j];
      if (!IsValidContinuationByte(continuation_byte)) {
        return false;
      }
      codepoint = (codepoint << 6) | (continuation_byte & 0x3F);
    }

    // 验证码点范围和编码规范性
    if (!IsValidCodepoint(codepoint, num_bytes)) {
      return false;
    }

    i += num_bytes;
  }

  return true;
}

}  // namespace base