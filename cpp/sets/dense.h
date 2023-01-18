#pragma once

#include "vendor/unordered_dense.h"

#include "../vec3.h"

template <typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct Vec3Hash {
  using is_avalanching = void;

  [[nodiscard]] auto operator()(Vec3<T> const &vec) const noexcept -> uint64_t {
    return ((ankerl::unordered_dense::detail::wyhash::hash(vec.x) ^
             (ankerl::unordered_dense::detail::wyhash::hash(vec.y) << 1)) >> 1) ^
           (ankerl::unordered_dense::detail::wyhash::hash(vec.z) << 1);
  }
};

typedef ankerl::unordered_dense::set<Vec3i, Vec3Hash<int>> Vec3Set;
typedef ankerl::unordered_dense::set<Vec3f, Vec3Hash<float>> Vec3fSet;
