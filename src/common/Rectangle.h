#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#include "Point.h"

class Rectangle {
public:
    Point leftDown;
    Point rightUp;

    Rectangle(int x1, int y1, int x2, int y2) : leftDown(x1, y1), rightUp(x2, y2) {}
    Rectangle(Point& leftDown_, Point& rightUp_) : leftDown(leftDown_), rightUp(rightUp_) {}
};

#endif // RECTANGLE_HPP