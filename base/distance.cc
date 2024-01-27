// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/distance.h"

#include <math.h>

#include "base/common.h"

namespace base {

Code GetGeoDistanceBase(double lat_x, double lon_x, double lat_y, double lon_y,
                        double *distance) { /*{{{*/
  if (distance == NULL) return kInvalidParam;

  if (lat_x == lat_y && lon_x == lon_y) {
    *distance = 0.0;
    return kOk;
  }

  *distance = kEarthRadius *
              acos(cos(ToRadian(lat_x)) * cos(ToRadian(lat_y)) * cos(ToRadian(lon_y - lon_x)) +
                   sin(ToRadian(lat_x)) * sin(ToRadian(lat_y)));
  return kOk;
} /*}}}*/

Code GetGeoDistanceHaversine(double lat_x, double lon_x, double lat_y, double lon_y,
                             double *distance) { /*{{{*/
  if (distance == NULL) return kInvalidParam;

  if (lat_x == lat_y && lon_x == lon_y) {
    *distance = 0.0;
    return kOk;
  }

  double hsin_x = sin(ToRadian(lon_x - lon_y) / 2);
  double hsin_y = sin(ToRadian(lat_x - lat_y) / 2);
  double h = hsin_y * hsin_y + cos(ToRadian(lat_x)) * cos(ToRadian(lat_y)) * hsin_x * hsin_x;
  if (h > 1) h = 1;

  *distance = 2 * atan2(sqrt(h), sqrt(1 - h)) * kEarthRadius;

  return kOk;
} /*}}}*/

Code GetGeoDistanceShort(double lat_x, double lon_x, double lat_y, double lon_y,
                         double *distance) { /*{{{*/
  if (distance == NULL) return kInvalidParam;

  if (lat_x == lat_y && lon_x == lon_y) {
    *distance = 0.0;
    return kOk;
  }

  double north_2_sourth = kEarthRadius * ToRadian(lat_x - lat_y);
  double west_2_east = kEarthRadius * cos(ToRadian(lat_x + lat_y) / 2) * ToRadian(lon_x - lon_y);

  *distance = sqrt(north_2_sourth * north_2_sourth + west_2_east * west_2_east);
  return kOk;
} /*}}}*/

double ToRadian(double x) { /*{{{*/
  return x / 180 * M_PI;
} /*}}}*/

}  // namespace base
