// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/utf8.h"
#include <cstddef>
#include <cstdint>

namespace base {

bool IsUtf8(const std::string& str) {
  const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
  size_t len = str.length();
  size_t i = 0;

  while (i < len) {
    unsigned char byte = bytes[i];

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
      // 110xxxxx: 2字节序列
      num_bytes = 2;
      codepoint = byte & 0x1F;
    } else if ((byte & 0xF0) == 0xE0) {
      // 1110xxxx: 3字节序列
      num_bytes = 3;
      codepoint = byte & 0x0F;
    } else if ((byte & 0xF8) == 0xF0) {
      // 11110xxx: 4字节序列
      num_bytes = 4;
      codepoint = byte & 0x07;
    } else {
      // 无效的起始字节
      return false;
    }

    // 检查是否有足够的字节
    if (i + num_bytes > len) {
      return false;
    }

    // 处理后续字节 (10xxxxxx)
    for (int j = 1; j < num_bytes; j++) {
      if (i + j >= len) {
        return false;
      }

      unsigned char continuation_byte = bytes[i + j];
      if ((continuation_byte & 0xC0) != 0x80) {
        return false;
      }

      codepoint = (codepoint << 6) | (continuation_byte & 0x3F);
    }

    // 验证码点范围和编码规范性
    if (num_bytes == 2) {
      // 2字节序列: U+0080 到 U+07FF
      if (codepoint < 0x80 || codepoint > 0x7FF) {
        return false;
      }
    } else if (num_bytes == 3) {
      // 3字节序列: U+0800 到 U+FFFF
      if (codepoint < 0x800 || codepoint > 0xFFFF) {
        return false;
      }
      // 检查代理对范围 (U+D800-U+DFFF)，UTF-8中不允许
      if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
        return false;
      }
    } else if (num_bytes == 4) {
      // 4字节序列: U+10000 到 U+10FFFF
      if (codepoint < 0x10000 || codepoint > 0x10FFFF) {
        return false;
      }
    }

    i += num_bytes;
  }

  return true;
}

}  // namespace base