#ifndef POINT_HPP
#define POINT_HPP

class Point {
public:
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
};

#endif // POINT_HPP