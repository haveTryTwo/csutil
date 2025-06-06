// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/mutable_buffer.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

namespace base {
MutableBuffer::MutableBuffer(uint32_t min_buf_size, uint32_t max_buf_size)
    : min_buf_size_(min_buf_size), max_buf_size_(max_buf_size) { /*{{{*/
  buf_ = NULL;
  buf_size_ = 0;
  cur_buf_size_ = 0;
  offset_ = 0;
} /*}}}*/

MutableBuffer::~MutableBuffer() { /*{{{*/
  if (buf_ != NULL) {
    free(buf_);
    buf_ = NULL;
  }
} /*}}}*/

/**
 * Note: when error code return, the caller should close current connection for losing some data for
 * current connection; then another connection should be established and new data would be sent
 * again
 */
Code MutableBuffer::Append(const char *data, uint32_t data_len) { /*{{{*/
  if (data == NULL) return kInvalidParam;
  if (buf_size_ + data_len > max_buf_size_)
    return kInvalidLength;  // Note: No space for current data, and the caller should close current
                            // connection for losing some data

  if (buf_ == NULL) {
    uint32_t alloc_size = data_len < min_buf_size_ ? min_buf_size_ : (min_buf_size_ + data_len);
    buf_ = (char *)malloc(alloc_size);
    if (buf_ == NULL) {
      return kMallocFailed;
    }

    buf_size_ = alloc_size;
    cur_buf_size_ = 0;
    offset_ = 0;
  }

  if ((buf_size_ - offset_ - cur_buf_size_) >= data_len) {
    memcpy(buf_ + offset_ + cur_buf_size_, data, data_len);
    cur_buf_size_ += data_len;
    return kOk;
  }

  if ((buf_size_ - cur_buf_size_) >= data_len) {
    memmove(buf_, buf_ + offset_, cur_buf_size_);
    memmove(buf_ + cur_buf_size_, data, data_len);
    cur_buf_size_ += data_len;
    offset_ = 0;

    return kOk;
  }

  uint32_t alloc_size = min_buf_size_ + buf_size_ + data_len;
  char *tmp_buf = (char *)malloc(alloc_size);
  if (tmp_buf == NULL) {
    return kMallocFailed;
  }

  memcpy(tmp_buf, buf_ + offset_, cur_buf_size_);
  memcpy(tmp_buf + cur_buf_size_, data, data_len);

  free(buf_);
  buf_ = tmp_buf;
  buf_size_ = alloc_size;
  cur_buf_size_ += data_len;
  offset_ = 0;

  return kOk;
} /*}}}*/

Code MutableBuffer::Skip(uint32_t skip_len) { /*{{{*/
  if (buf_ == NULL) return kOk;

  if (skip_len >= cur_buf_size_) {
    if (buf_size_ > min_buf_size_) {
      free(buf_);
      buf_ = NULL;
      buf_size_ = 0;
    }
    cur_buf_size_ = 0;
    offset_ = 0;

    return kOk;
  }

  offset_ += skip_len;
  cur_buf_size_ -= skip_len;

  return kOk;
} /*}}}*/

const char *MutableBuffer::RealDataPtr() { /*{{{*/
  if (buf_ == NULL) return NULL;

  return buf_ + offset_;
} /*}}}*/

uint32_t MutableBuffer::RealDataSize() { /*{{{*/ return cur_buf_size_; } /*}}}*/

}  // namespace base
