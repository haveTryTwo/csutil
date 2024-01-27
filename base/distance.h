// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CPU_H_
#define BASE_CPU_H_

#include <sched.h>

#include <set>

#include "base/status.h"

namespace base {

/**
 * Get the distance between x and y on the earth
 * Coordinate of x is <latitude_x, longitude_x>, and the same is y
 */
Code GetGeoDistanceBase(double lat_x, double lon_x, double lat_y, double lon_y, double *distance);

Code GetGeoDistanceHaversine(double lat_x, double lon_x, double lat_y, double lon_y,
                             double *distance);

Code GetGeoDistanceShort(double lat_x, double lon_x, double lat_y, double lon_y, double *distance);

double ToRadian(double x);
}  // namespace base

#endif
