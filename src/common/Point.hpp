#ifndef POINT_HPP
#define POINT_HPP

class Point {
 public:
  long x;
  long y;

  Point(long x_ = 0, long y_ = 0) : x(x_), y(y_) {}
};

#endif  // POINT_HPP