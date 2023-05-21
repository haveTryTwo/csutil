// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <math.h>
#include <stdio.h>

#include "base/common.h"
#include "base/distance.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(Distance, Test_GeoDistanceBase_Normal) { /*{{{*/
  using namespace base;
  double arr[] = {39.941, 116.45, 39.94, 116.451, 43.4,  138.4,  43.5,  138.5,
                  44,     138,    45,    139,     39.26, 115.25, 41.04, 117.30};

  for (int i = 0; i < (int)sizeof(arr) / sizeof(arr[0]); i = i + 4) {
    double lat_x = arr[i];
    double lon_x = arr[i + 1];
    double lat_y = arr[i + 2];
    double lon_y = arr[i + 3];
    double distance = 0.0;
    Code ret = GetGeoDistanceBase(lat_x, lon_x, lat_y, lon_y, &distance);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "<%f, %f> and <%f, %f> distance:%f\n", lat_x, lon_x, lat_y, lon_y, distance);
  }
} /*}}}*/

TEST(Distance, Test_GeoDistanceHaversine_Normal) { /*{{{*/
  using namespace base;
  double arr[] = {39.941, 116.45, 39.94, 116.451, 43.4,  138.4,  43.5,  138.5,
                  44,     138,    45,    139,     39.26, 115.25, 41.04, 117.30};

  for (int i = 0; i < (int)sizeof(arr) / sizeof(arr[0]); i = i + 4) {
    double lat_x = arr[i];
    double lon_x = arr[i + 1];
    double lat_y = arr[i + 2];
    double lon_y = arr[i + 3];
    double distance = 0.0;
    Code ret = GetGeoDistanceHaversine(lat_x, lon_x, lat_y, lon_y, &distance);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "<%f, %f> and <%f, %f> distance:%f\n", lat_x, lon_x, lat_y, lon_y, distance);
  }
} /*}}}*/

TEST(Distance, Test_GeoDistanceShort_Normal) { /*{{{*/
  using namespace base;
  double arr[] = {39.941, 116.45, 39.94, 116.451, 43.4,  138.4,  43.5,  138.5,
                  44,     138,    45,    139,     39.26, 115.25, 41.04, 117.30};

  for (int i = 0; i < (int)sizeof(arr) / sizeof(arr[0]); i = i + 4) {
    double lat_x = arr[i];
    double lon_x = arr[i + 1];
    double lat_y = arr[i + 2];
    double lon_y = arr[i + 3];
    double distance = 0.0;
    Code ret = GetGeoDistanceShort(lat_x, lon_x, lat_y, lon_y, &distance);
    EXPECT_EQ(kOk, ret);
    fprintf(stderr, "<%f, %f> and <%f, %f> distance:%f\n", lat_x, lon_x, lat_y, lon_y, distance);
  }
} /*}}}*/
