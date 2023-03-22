#ifndef NAIVE_RECTANGLE_ENUMERATION_HPP
#define NAIVE_RECTANGLE_ENUMERATION_HPP

#include <vector>
#include "AbstractImplementation.h"

class NaiveRectangleEnumeration : AbstractImplementation {
public:
    NaiveRectangleEnumeration(std::vector<Rectangle>&& rectangles);
    void buildInternals();
    int queryPoint(const Point& point);
};

#endif // NAIVE_RECTANGLE_ENUMERATION_HPP