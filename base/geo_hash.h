// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CODING_H_
#define BASE_CODING_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "base/status.h"

namespace base {

// geohash: Convert string to longitude and latitude interval and center point
struct GeoBox {
  double min_lat = 0;
  double max_lat = 0;
  double min_lon = 0;
  double max_lon = 0;
  double CenterLat() const { return (min_lat + max_lat) / 2.0; }
  double CenterLon() const { return (min_lon + max_lon) / 2.0; }
};

struct GeoPoint {
  double lat = 0;
  double lon = 0;
};

// precision chosen:
// Global or national level: length is 1~3, with an error range of tens of kilometers to hundreds of kilometers.
// City or regional level: length is 4~6, with an error range of several kilometers to hundreds of meters.
// Street or building level: length is 7~9, with an error range of tens of meters to several meters.
// Indoor or high-precision positioning: length is 10~12, with an error range of centimeters.
Code GeoHashEncode(double latitude, double longitude, int precision, std::string* encode_value);
GeoBox GeoHashDecode(const std::string& geohash);

std::string GeoHashNeighbor(const std::string& geohash, const std::string& direction);
std::unordered_map<std::string, std::string> GeoHashNeighbors(const std::string& geohash);

}  // namespace base
#endif  // BASE_CODING_H_
