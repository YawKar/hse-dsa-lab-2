#include <benchmark/benchmark.h>

#include <iostream>

#include "common/TestCaseGenerator.hpp"
#include "implementations/NaiveRectangleEnumeration.hpp"
#include "implementations/PersistentSegmentTree.hpp"
#include "implementations/QubicMapBuilding.hpp"

const int XSEED = 42;
const int YSEED = 13;

static std::vector<Rectangle> getRectangles(long numberOfRectangles) {
  return TestCaseGenerator::generateRecommendedRectangles(static_cast<int>(numberOfRectangles));
}

static std::vector<Point> getPoints(long numberOfPoints) {
  return TestCaseGenerator::generateUniformlyDistributedPoints(static_cast<int>(numberOfPoints), 0,
                                                               static_cast<int>(numberOfPoints) * 20, 0,
                                                               static_cast<int>(numberOfPoints) * 20, XSEED, YSEED);
}

// NaiveRectangleEnumeration
static void BM_Building_NaiveRectangleEnumeration(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto rectangles = getRectangles(state.range(0));
    auto algorithm = NaiveRectangleEnumeration(std::move(rectangles));
    state.ResumeTiming();

    algorithm.buildInternals();
  }
}

BENCHMARK(BM_Building_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(10, 100, 10)
    ->Iterations(100000);
BENCHMARK(BM_Building_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(200, 1000, 100)
    ->Iterations(100000);
BENCHMARK(BM_Building_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(10000, 100000, 10000)
    ->Iterations(10000);

static void BM_PerRequest_NaiveRectangleEnumeration(benchmark::State &state) {
  auto rectangles = getRectangles(state.range(0));
  auto points = getPoints(state.range(0));
  std::size_t currentIdx = 0;
  auto algorithm = NaiveRectangleEnumeration(std::move(rectangles));
  algorithm.buildInternals();

  for (auto _ : state) {
    algorithm.queryPoint(points[currentIdx++]);
    state.PauseTiming();
    currentIdx %= points.size();
    state.ResumeTiming();
  }
}

BENCHMARK(BM_PerRequest_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(10, 100, 10)
    ->Iterations(100000);
BENCHMARK(BM_PerRequest_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(200, 1000, 100)
    ->Iterations(100000);
BENCHMARK(BM_PerRequest_NaiveRectangleEnumeration)
    ->ArgName("Rectangles&Points")
    ->DenseRange(10000, 100000, 10000)
    ->Iterations(10000);

// QubicMapBuilding
static void BM_Building_QubicMapBuilding(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto rectangles = getRectangles(state.range(0));
    auto algorithm = QubicMapBuilding(std::move(rectangles));
    state.ResumeTiming();

    algorithm.buildInternals();
  }
}

BENCHMARK(BM_Building_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10);
BENCHMARK(BM_Building_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(200, 1000, 100);
BENCHMARK(BM_Building_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(2000, 5000, 1000);

static void BM_PerRequest_QubicMapBuilding(benchmark::State &state) {
  auto rectangles = getRectangles(state.range(0));
  auto points = getPoints(state.range(0));
  std::size_t currentIdx = 0;
  auto algorithm = QubicMapBuilding(std::move(rectangles));
  algorithm.buildInternals();

  for (auto _ : state) {
    algorithm.queryPoint(points[currentIdx++]);
    state.PauseTiming();
    currentIdx %= points.size();
    state.ResumeTiming();
  }
}

BENCHMARK(BM_PerRequest_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10);
BENCHMARK(BM_PerRequest_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(200, 1000, 100);
BENCHMARK(BM_PerRequest_QubicMapBuilding)->ArgName("Rectangles&Points")->DenseRange(2000, 5000, 1000);

// PersistentSegmentTree
static void BM_Building_PersistentSegmentTree(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto rectangles = getRectangles(state.range(0));
    auto algorithm = PersistentSegmentTree(std::move(rectangles));
    state.ResumeTiming();

    algorithm.buildInternals();
  }
}

BENCHMARK(BM_Building_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10);
BENCHMARK(BM_Building_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(200, 1000, 100);
BENCHMARK(BM_Building_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(10000, 100000, 10000);

static void BM_PerRequest_PersistentSegmentTree(benchmark::State &state) {
  auto rectangles = getRectangles(state.range(0));
  auto points = getPoints(state.range(0));
  std::size_t currentIdx = 0;
  auto algorithm = PersistentSegmentTree(std::move(rectangles));
  algorithm.buildInternals();

  for (auto _ : state) {
    algorithm.queryPoint(points[currentIdx++]);
    state.PauseTiming();
    currentIdx %= points.size();
    state.ResumeTiming();
  }
}

BENCHMARK(BM_PerRequest_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(10, 100, 10);
BENCHMARK(BM_PerRequest_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(200, 1000, 100);
BENCHMARK(BM_PerRequest_PersistentSegmentTree)->ArgName("Rectangles&Points")->DenseRange(10000, 100000, 10000);

BENCHMARK_MAIN();