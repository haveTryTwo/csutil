// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CODING_H_
#define BASE_CODING_H_

#include <stdint.h>

#include <deque>
#include <string>
#include <vector>

#include "status.h"

namespace base {

Code EncodeFixed32(uint32_t num, std::string *out);
Code DecodeFixed32(const std::string &in, uint32_t *value);

Code EncodeFixed64(uint64_t num, std::string *out);
Code DecodeFixed64(const std::string &in, uint64_t *value);

Code EncodeVar32(uint32_t num, std::string *out);
Code DecodeVar32(const std::string &in, uint32_t *value);

Code EncodeVar64(uint64_t num, std::string *out);
Code DecodeVar64(const std::string &in, uint64_t *value);

/**
 * see:https://developers.google.com/protocol-buffers/docs/encoding#types for zigzag
 * This is useful for saving storage of negative data when using EncodeVar32/EncodeVar64 coding;
 */
Code EncodeZigZag32(int num, uint32_t *out);
Code DecodeZigZag32(uint32_t in, int *num);

Code EncodeZigZag64(int64_t num, uint64_t *out);
Code DecodeZigZag64(uint64_t in, int64_t *num);

// Example: 0x1 = > '1'
Code ToHex(uint8_t src_ch, uint8_t *dst_hex_ch);
// Example: '1' => 0x1
Code ToBin(uint8_t src_hex_ch, uint8_t *dst_ch);

// Example: 0xa1 => 'a''1'
Code BinToAscii(uint8_t src_ch, uint8_t *dst_high_ch, uint8_t *dst_low_ch);
// Example: 'a''1' => 0xa1
Code AsciiToBin(uint8_t src_high_ch, uint8_t src_low_ch, uint8_t *dst_bin_ch);

/**
 * Note: 1. Using standard of https://en.wikipedia.org/wiki/Percent-encoding
 *       2. As is known that Characters from the unreserved set never need to be percent-encoded!
 *          Unrserved characters are:
 *          A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
 *          a b c d e f g h i j k l m n o p q r s t u v w x y z
 *          0 1 2 3 4 5 6 7 8 9 - _ . ~
 */
Code UrlEncode(const std::string &src, std::string *dst);

Code UrlDecode(const std::string &src, std::string *dst);

Code Base64Encode(const std::string &src, std::string *dst);
Code Base64Decode(const std::string &src, std::string *dst);

Code Base16Encode(const std::string &src, std::string *dst);
Code Base16Decode(const std::string &src, std::string *dst);

Code Base32Encode(const std::string &src, std::string *dst);
Code Base32Decode(const std::string &src, std::string *dst);
Code Base32EncodeForGeoHash(const std::string &src, std::string *dst);
Code Base32DecodeForGeoHash(const std::string &src, std::string *dst);

// find the first postion of needle in haystack using BruteForce
Code BruteForce(const std::string &haystack, const std::string &needle, int *pos);

// find the first postion of needle in haystack using Knuth-Morris-Pratt
Code KMP(const std::string &haystack, const std::string &needle, int *pos);

// find the first postion of needle in haystack using Boyer-Moore
Code BM(const std::string &haystack, const std::string &needle, int *pos);

// find the first postion of needle in haystack using Rabin-Karp
Code RK(const std::string &haystack, const std::string &needle, int *pos);

// get highlight string which will be linked by pre_tags and post_tags
// pre_tags may be <em> and post_tags may be </em>
Code GetHighlighting(const std::string &haystack, const std::string &needle, int pos, const std::string &pre_tags,
                     const std::string &post_tags, std::string *hightlight);

// Split characters by utf8 encoding, and if English then by word
Code SplitUTF8(const std::string &src, std::deque<std::string> *out);

// reference: https://github.com/facebook/mysql-5.6/wiki/MyRocks-record-format#memcomparable-format
Code MemcomparableEncode(const std::string &src, std::string *dst);
Code MemcomparableDecode(const std::string &src, std::string *dst);

}  // namespace base

#endif
