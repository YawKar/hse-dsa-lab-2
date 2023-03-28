#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#include "Point.hpp"

class Rectangle {
 public:
  Point leftDown;
  Point rightUp;

  Rectangle(long x1, long y1, long x2, long y2) : leftDown(x1, y1), rightUp(x2, y2) {}
  Rectangle(const Point& leftDown_, const Point& rightUp_) : leftDown(leftDown_), rightUp(rightUp_) {}
};

#endif  // RECTANGLE_HPP