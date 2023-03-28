#ifndef TEST_CASE_GENERATOR_HPP
#define TEST_CASE_GENERATOR_HPP

#include <random>
#include <vector>

#include "Rectangle.hpp"

class TestCaseGenerator {
 public:
  static std::vector<Rectangle> generateRecommendedRectangles(int rectangles);
  static std::vector<Point> generateUniformlyDistributedPoints(
      int numberOfPoints, int minX, int maxX, int minY, int maxY,
      int xSeed = std::random_device()(), int ySeed = std::random_device()());
};

#endif  // TEST_CASE_GENERATOR_HPP