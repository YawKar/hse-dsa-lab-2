#include <benchmark/benchmark.h>
#include <iostream>
#include "implementations/NaiveRectangleEnumeration.h"
#include "common/TestCaseGenerator.h"

const int XSEED = 42;
const int YSEED = 13;

static void BM_NaiveRectangleEnumeration(benchmark::State& state) {
  for (auto _ : state) {
    
    state.PauseTiming();
    auto rectangles = TestCaseGenerator::generateRecommendedRectangles(state.range(0));
    auto points = TestCaseGenerator::generateUniformlyDistributedPoints(state.range(0), 0, state.range(0) * 20, 0, state.range(0) * 20, XSEED, YSEED);
    auto algorithm = NaiveRectangleEnumeration(std::move(rectangles));
    state.ResumeTiming();
    
    algorithm.buildInternals();
    for (const auto& point : points) {
      algorithm.queryPoint(point);
    }
  }
}

BENCHMARK(BM_NaiveRectangleEnumeration)->ArgName("Rectangles&Points")->DenseRange(100, 1000, 100);

BENCHMARK_MAIN();