#include "absl/container/flat_hash_set.h"
#include <absl/hash/hash.h>
#include <absl/hash/internal/hash.h>
#include <absl/numeric/int128.h>

#include "../vec3.h"

typedef absl::flat_hash_set<Vec3i> Vec3Set;
typedef absl::flat_hash_set<Vec3f> Vec3fSet;
