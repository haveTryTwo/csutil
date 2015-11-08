// Copyright (c) 2015 The CCUtil Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_HAHS_H_
#define BASE_HAHS_H_

#include <stdint.h>

namespace base
{

/**
 * BKDR Hash 
 */
uint32_t BKDRHash(const char *str);

uint32_t BKDRHash(uint32_t num);

}

#endif
