#include "benchmark/benchmark.h"
#include "part2.cpp"
#include <sstream>
#include <stdexcept>

static void BM_Run(benchmark::State &state) {
  for (auto _ : state) {
    int res = run();
    if (res != 58) {
      std::stringstream ss;
      ss << "Invalid result ";
      ss << res;
      throw std::runtime_error(ss.str());
    }
  }
}

BENCHMARK(BM_Run);
BENCHMARK_MAIN();
