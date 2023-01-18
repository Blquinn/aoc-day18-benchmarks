#pragma once

#include <unordered_set>

#include "../vec3.h"

template <typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct Vec3Hash {
  const std::hash<T> hash = std::hash<T>();

  auto operator()(const Vec3<T> &vec) const -> std::size_t {
    return ((hash(vec.x) ^ (hash(vec.y) << 1)) >> 1) ^ (hash(vec.z) << 1);
  }
};

typedef std::unordered_set<Vec3i, Vec3Hash<int>> Vec3Set;
typedef std::unordered_set<Vec3f, Vec3Hash<float>> Vec3fSet;
