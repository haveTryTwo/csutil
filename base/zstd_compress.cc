// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/zstd_compress.h"

#include "zstd.h"

namespace base {

Code ZstdCompress::Compress(const std::string &uncompressed, std::string *compressed) {/*{{{*/
  if (compressed == NULL) return kInvalidParam;

  size_t compress_len = ZSTD_compressBound(uncompressed.size());
  if (ZSTD_isError(compress_len) != 0) return kFailedToGetCompressLen;

  char *zstd_compressed_data = new char[compress_len];
  if (zstd_compressed_data == NULL) return kNewFailed;

  size_t ret_compress_len = ZSTD_compress(zstd_compressed_data, compress_len, uncompressed.data(),
                                          uncompressed.size(), 0);
  if (ZSTD_isError(ret_compress_len) != 0) {
    delete[] zstd_compressed_data;
    return kFailedToCompress;
  }

  compressed->assign(zstd_compressed_data, ret_compress_len);

  delete[] zstd_compressed_data;

  return kOk;
}/*}}}*/

Code ZstdCompress::Uncompress(const std::string &compressed, std::string *uncompressed) {/*{{{*/
  if (uncompressed == NULL) return kInvalidParam;

  unsigned long long const uncompress_len =
      ZSTD_getFrameContentSize(compressed.data(), compressed.size());
  if (uncompress_len == ZSTD_CONTENTSIZE_ERROR || uncompress_len == ZSTD_CONTENTSIZE_UNKNOWN) {
    return kFailedToGetUnCompressLen;
  }

  char *zstd_uncompress_data = new char[uncompress_len];
  if (zstd_uncompress_data == NULL) return kNewFailed;

  size_t ret_uncompress_len =
      ZSTD_decompress(zstd_uncompress_data, uncompress_len, compressed.data(), compressed.size());
  if (ZSTD_isError(ret_uncompress_len) != 0) {
    delete[] zstd_uncompress_data;
    return kFailedToUncompress;
  }

  uncompressed->assign(zstd_uncompress_data, ret_uncompress_len);

  delete[] zstd_uncompress_data;

  return kOk;
}/*}}}*/

}  // namespace base
