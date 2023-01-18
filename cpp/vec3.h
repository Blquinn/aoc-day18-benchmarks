#pragma once

#include <ostream>
#include <tuple>

#include "defs.h"

template <typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct Vec3 {
  T x;
  T y;
  T z;

  bool operator==(const Vec3 &b) const {
    return x == b.x && y == b.y && z == b.z;
  }

  bool operator<(const Vec3 &b) const {
    return std::tie(x, y, z) < std::tie(b.x, b.y, b.z);
  }

  Vec3 operator+(const Vec3<T> &r) { return {x + r.x, y + r.y, z + r.z}; }

  friend std::ostream &operator<<(std::ostream &os, const Vec3 &vec) {
    os << '{' << vec.x << ',' << vec.y << ',' << vec.z << '}';
    return os;
  }

// #ifdef USE_ABSL_FLAT_SET
  template <typename H>
  friend H AbslHashValue(H h, const Vec3& vec) {
    return H::combine(std::move(h), vec.x, vec.y, vec.z);
  }
// #endif
};

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
