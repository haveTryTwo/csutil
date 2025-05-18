// Copyright (c) 2015 The CSUTIL Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "geo_hash.h"

#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace base {

const std::string kBase32 = "0123456789bcdefghjkmnpqrstuvwxyz";

// Neighbor Table and Boundary Table
const std::unordered_map<std::string, std::vector<std::string>> neighbor = {
    {"n", {"p0r21436x8zb9dcf5h7kjnmqesgutwvy", "bc01fg45238967deuvhjyznpkmstqrwx"}},
    {"s", {"14365h7k9dcfesgujnmqp0r2twvyx8zb", "238967debc01fg45kmstqrwxuvhjyznp"}},
    {"e", {"bc01fg45238967deuvhjyznpkmstqrwx", "p0r21436x8zb9dcf5h7kjnmqesgutwvy"}},
    {"w", {"238967debc01fg45kmstqrwxuvhjyznp", "14365h7k9dcfesgujnmqp0r2twvyx8zb"}}};

const std::unordered_map<std::string, std::vector<std::string>> border = {
    {"n", {"prxz", "bcfguvyz"}}, {"s", {"028b", "0145hjnp"}}, {"e", {"bcfguvyz", "prxz"}}, {"w", {"0145hjnp", "028b"}}};


Code GeoHashEncode(double latitude, double longitude, int precision, std::string* encode_value) {/*{{{*/
  if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0 ||
      precision < 1 || precision > 12 || encode_value == NULL) {
    return kInvalidParam;
  }

  double lat_interval[] = {-90.0, 90.0};
  double lon_interval[] = {-180.0, 180.0};
  std::string geohash;
  bool is_even = true;
  int bit = 0, ch = 0;

  while (geohash.length() < (size_t)precision) {
    double mid = 0;
    if (is_even) {
      mid = (lon_interval[0] + lon_interval[1]) / 2.0;
      if (longitude > mid) {
        ch |= (1 << (4 - bit));
        lon_interval[0] = mid;
      } else {
        lon_interval[1] = mid;
      }
    } else {
      mid = (lat_interval[0] + lat_interval[1]) / 2.0;
      if (latitude > mid) {
        ch |= (1 << (4 - bit));
        lat_interval[0] = mid;
      } else {
        lat_interval[1] = mid;
      }
    }
    is_even = !is_even;
    if (bit < 4) {
      bit++;
    } else {
      if (ch >= kBase32.size()) return kInvalidStatus;
      geohash += kBase32[ch];
      bit = 0;
      ch = 0;
    }
  }

  *encode_value = geohash;

  return kOk;
}/*}}}*/

GeoBox GeoHashDecode(const std::string& geohash) { /*{{{*/
  double lat_interval[] = {-90.0, 90.0};
  double lon_interval[] = {-180.0, 180.0};
  bool is_even = true;

  for (char c : geohash) {
    int cd = kBase32.find(c);
    for (int mask = 16; mask > 0; mask >>= 1) {
      if (is_even) {
        double mid = (lon_interval[0] + lon_interval[1]) / 2.0;
        if (cd & mask)
          lon_interval[0] = mid;
        else
          lon_interval[1] = mid;
      } else {
        double mid = (lat_interval[0] + lat_interval[1]) / 2.0;
        if (cd & mask)
          lat_interval[0] = mid;
        else
          lat_interval[1] = mid;
      }
      is_even = !is_even;
    }
  }

  return {lat_interval[0], lat_interval[1], lon_interval[0], lon_interval[1]};
} /*}}}*/

// Calculate one-way neighbors
std::string GeoHashNeighbor(const std::string& geohash, const std::string& direction) { /*{{{*/
  if (geohash.empty()) return "";

  std::string dir = direction;
  if (dir != "n" && dir != "s" && dir != "e" && dir != "w") return "";

  int len = geohash.length();
  int is_even = len % 2 == 0 ? 0 : 1;
  char last_char = geohash[len - 1];
  std::string parent = geohash.substr(0, len - 1);

  // 检查是否在边界
  if (border.at(dir)[is_even].find(last_char) != std::string::npos && !parent.empty()) {
    parent = GeoHashNeighbor(parent, dir);
  }

  // 计算邻居字符
  size_t pos = neighbor.at(dir)[is_even].find(last_char);
  char neighbor_char = kBase32[pos];

  return parent + neighbor_char;
} /*}}}*/

// 8-way neighbors
std::unordered_map<std::string, std::string> GeoHashNeighbors(const std::string& geohash) { /*{{{*/
  std::unordered_map<std::string, std::string> result;
  result["n"] = GeoHashNeighbor(geohash, "n");
  result["s"] = GeoHashNeighbor(geohash, "s");
  result["e"] = GeoHashNeighbor(geohash, "e");
  result["w"] = GeoHashNeighbor(geohash, "w");
  result["ne"] = GeoHashNeighbor(result["n"], "e");
  result["nw"] = GeoHashNeighbor(result["n"], "w");
  result["se"] = GeoHashNeighbor(result["s"], "e");
  result["sw"] = GeoHashNeighbor(result["s"], "w");
  return result;
} /*}}}*/

}  // namespace base
