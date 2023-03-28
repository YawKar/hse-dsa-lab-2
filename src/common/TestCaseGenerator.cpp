#include "TestCaseGenerator.h"

std::vector<Rectangle> TestCaseGenerator::generateRecommendedRectangles(
    int numberOfRectangles) {
  std::vector<Rectangle> rects;
  rects.reserve(numberOfRectangles);
  for (int i = 0; i < numberOfRectangles; ++i) {
    rects.emplace_back(10 * i, 10 * i, 10 * (2 * numberOfRectangles - i),
                       10 * (2 * numberOfRectangles - i));
  }
  return rects;
}

std::vector<Point> TestCaseGenerator::generateUniformlyDistributedPoints(
    int numberOfPoints, int minX, int maxX, int minY, int maxY, int xSeed,
    int ySeed) {
  std::mt19937 xEngine(xSeed);
  std::mt19937 yEngine(ySeed);
  std::uniform_int_distribution<int> xDist(minX, maxX);
  std::uniform_int_distribution<int> yDist(minY, maxY);

  std::vector<Point> points;
  points.reserve(numberOfPoints);
  for (int i = 0; i < numberOfPoints; ++i) {
    points.emplace_back(xDist(xEngine), yDist(yEngine));
  }
  return points;
}