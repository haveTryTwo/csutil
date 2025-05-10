// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "base/common.h"
#include "base/geo_hash.h"
#include "base/random.h"
#include "base/status.h"

#include "test_base/include/test_base.h"

TEST(GeoHash, Test_Normal_geohash) { /*{{{*/
  using namespace base;

  std::vector<GeoPoint> points = {
      {0.908823, 0.397470}, {0.908823, -0.397470}, {-0.908823, 0.397470}, {-0.908823, -0.397470}};
  for (GeoPoint geo_point : points) {
    std::cout << "Point: {" << geo_point.lat << "," << geo_point.lon << "}" << "\n";
    std::string geohash;
    base::Code ret = GeoHashEncode(geo_point.lat, geo_point.lon, 12, &geohash);
    EXPECT_EQ(ret, kOk);
    std::cout << "Encode: " << geohash << std::endl;

    GeoBox box = GeoHashDecode(geohash);
    std::cout << "Decode: center=(" << box.CenterLat() << "," << box.CenterLon() << ")\n";
    std::cout << "Range: lat[" << box.min_lat << "," << box.max_lat << "], lon[" << box.min_lon << "," << box.max_lon
              << "]\n";

    auto neighbors = GeoHashNeighbors(geohash);
    std::cout << "Neighbors:\n";
    for (const auto& kv : neighbors) {
      std::cout << kv.first << ": " << kv.second << std::endl;
    }
    std::cout << "\n";
  }
} /*}}}*/

TEST(GeoHash, Test_Exception_geohash) { /*{{{*/
  using namespace base;
  std::string geohash;

  // Test invalid latitude (outside [-90, 90])
  Code ret = GeoHashEncode(91.0, 116.397470, 12, &geohash);
  EXPECT_EQ(ret, kInvalidParam);
  
  ret = GeoHashEncode(-91.0, 116.397470, 12, &geohash);
  EXPECT_EQ(ret, kInvalidParam);

  // Test invalid longitude (outside [-180, 180])
  ret = GeoHashEncode(39.908823, 181.0, 12, &geohash);
  EXPECT_EQ(ret, kInvalidParam);
  
  ret = GeoHashEncode(39.908823, -181.0, 12, &geohash);
  EXPECT_EQ(ret, kInvalidParam);

  // Test invalid precision (assuming valid range is something like 1-32)
  ret = GeoHashEncode(39.908823, 116.397470, 0, &geohash);
  EXPECT_EQ(ret, kInvalidParam);
  
  ret = GeoHashEncode(39.908823, 116.397470, 33, &geohash);
  EXPECT_EQ(ret, kInvalidParam);

  // Test null pointer for output
  ret = GeoHashEncode(39.908823, 116.397470, 12, nullptr);
  EXPECT_EQ(ret, kInvalidParam);
} /*}}}*/
