#include <algorithm>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "vec3.h"

#ifdef USE_HASHSET
#include <sets/hashset.h>
#elif defined(USE_ANKRL)
#include "sets/dense.h"
#elif defined(USE_BTREE)
#include "sets/btree.h"
#elif defined(USE_ABSL_FLAT_SET)
#include "sets/absl_flat_set.h"
#else
#include <sets/rbtree.h>
#endif


struct Calculator {
  Vec3Set cubes;
  Vec3fSet sides;
  Vec3Set lavaCubes;

  int minX, maxX;
  int minY, maxY;
  int minZ, maxZ;

  void printCubes() {
    std::ofstream f("PARSE");
    for (auto &&cube : cubes)
      f << cube.x - 1 << ',' << cube.y - 1 << ',' << cube.z - 1 << std::endl;
  }

  Vec3i getOppositeCube(const Vec3f &side) {
    float diffs[] = {-0.5, 0.5};
    bool isX = side.x != (int)side.x;
    bool isY = side.y != (int)side.y;
    bool isZ = side.z != (int)side.z;
    for (float diff : diffs) {
      Vec3i cube = {int(side.x + isX * diff), int(side.y + isY * diff),
                    int(side.z + isZ * diff)};

      if (cubes.count(cube) > 0) {
        return {int(side.x + isX * -diff), int(side.y + isY * -diff),
                int(side.z + isZ * -diff)};
      }
    }

    throw std::runtime_error("Cube not found");
  }

  bool isLavaSide(const Vec3f &side) {
    return lavaCubes.contains(getOppositeCube(side));
  }

  void dfsLava(Vec3i lava) {
    // Off limits
    if (lava.x > maxX + 1 || lava.x < minX - 1 || lava.y > maxY + 1 ||
        lava.y < minY - 1 || lava.z > maxZ + 1 || lava.z < minZ - 1)
      return;

    // Can't be both lava and non-lava
    if (cubes.contains(lava))
      return;

    // Already inserted
    if (!lavaCubes.insert(lava).second)
      return;

    static const Vec3i differs[] = {{-1, 0, 0}, {1, 0, 0},  {0, -1, 0},
                                    {0, 1, 0},  {0, 0, -1}, {0, 0, 1}};

    for (auto &&differ : differs) {
      dfsLava(lava + differ);
    }
  }

  // Mark the corner as a lava cube
  // DFS from there
  void populateLavaCubes() {
    // Mark the corner as a lava cube
    auto cube = *cubes.begin();
    minX = maxX = cube.x;
    minY = maxY = cube.y;
    minZ = maxZ = cube.z;

    for (auto &&cube : cubes) {
      minX = std::min(minX, cube.x);
      maxX = std::max(maxX, cube.x);

      minY = std::min(minY, cube.y);
      maxY = std::max(maxY, cube.y);

      minZ = std::min(minZ, cube.z);
      maxZ = std::max(maxZ, cube.z);
    }

    Vec3i lava = {minX - 1, minY - 1, minZ - 1};
    dfsLava(lava);
  }

  // Keep only valid sides
  void filterSides() {
    populateLavaCubes();

    // Filter out sides not exposed to lava
    for (auto it = sides.begin(); it != sides.end();) {
      auto side = *it;

      Vec3i a = {(int)side.x, (int)side.y, (int)side.z};
      Vec3i b = a;
      if (side.x != (int)side.x) {
        b.x++;
      } else if (side.y != (int)side.y) {
        b.y++;
      } else if (side.z != (int)side.z) {
        b.z++;
      } else {
        throw std::runtime_error("Invalid side");
      }

      bool containsA = cubes.contains(a);
      bool containsB = cubes.contains(b);

      // Can't both be zero
      if (!(containsA || containsB)) {
        throw std::runtime_error("Invalid side");
      }

      // Not exposed to air
      if (containsA && containsB) {
#ifdef USE_BTREE
        // For some reason this is required with abseil's btree implementation.
        it = sides.erase(it++);
#else
        sides.erase(it++);
#endif
        continue;
      }

      // Not exposed to lava
      if (!isLavaSide(side)) {
#ifdef USE_BTREE
        // For some reason this is required with abseil's btree implementation.
        it = sides.erase(it++);
#else
        sides.erase(it++);
#endif
        continue;
      }

      // Keep it
      it++;
    }
  }

  int getSurfaceArea() {
    for (auto &&cube : cubes) {
      const float diffs[] = {-0.5, 0.5};
      for (float diff : diffs) {
        sides.insert({(float)cube.x + diff, (float)cube.y, (float)cube.z});
        sides.insert({(float)cube.x, ((float)cube.y + diff), (float)cube.z});
        sides.insert({(float)cube.x, (float)cube.y, (float)cube.z + diff});
      }
    }
    filterSides();
    return sides.size();
  }
};

int run() {

// #define USE_STDIN
#ifdef USE_STDIN
  static std::istream &input = std::cin;
#else
  std::stringstream input = std::stringstream(R"(2,2,2
  1,2,2
  3,2,2
  2,1,2
  2,3,2
  2,2,1
  2,2,3
  2,2,4
  2,2,6
  1,2,5
  3,2,5
  2,1,5
  2,3,5)");
#endif

  Calculator calc;

  std::string line;
  while (std::getline(input, line)) {
    std::stringstream ss(line);

    Vec3i cube;
    char delim;
    ss >> cube.x >> delim >> cube.y >> delim >> cube.z;

    // Avoid 0
    cube.x++, cube.y++, cube.z++;
    calc.cubes.insert(cube);
  }

  // calc.printCubes();

  return calc.getSurfaceArea();
}
