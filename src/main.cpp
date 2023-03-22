#include <benchmark/benchmark.h>
#include <iostream>
#include "common/TestCaseGenerator.h"
#include "implementations/NaiveRectangleEnumeration.h"
#include "implementations/QubicMapBuilding.h"

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

BENCHMARK(BM_NaiveRectangleEnumeration)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10)->Iterations(10);
BENCHMARK(BM_NaiveRectangleEnumeration)->ArgName("Rectangles&Points")->DenseRange(100, 1000, 100)->Iterations(10);
BENCHMARK(BM_NaiveRectangleEnumeration)->ArgName("Rectangles&Points")->DenseRange(10000, 100000, 10000)->Iterations(3);

static void BM_QubicMapBuilding(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto rectangles = TestCaseGenerator::generateRecommendedRectangles(state.range(0));
    auto points = TestCaseGenerator::generateUniformlyDistributedPoints(state.range(0), 0, state.range(0) * 20, 0, state.range(0) * 20, XSEED, YSEED);
    auto algorithm = QubicMapBuilding(std::move(rectangles));
    state.ResumeTiming();
    
    algorithm.buildInternals();
    for (const auto& point : points) {
      algorithm.queryPoint(point);
    }
  }
}

BENCHMARK(BM_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10)->Iterations(10);
BENCHMARK(BM_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(100, 1000, 100)->Iterations(10);
BENCHMARK(BM_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(1000, 5000, 1000)->Iterations(2);

BENCHMARK_MAIN();