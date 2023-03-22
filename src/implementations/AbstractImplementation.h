#ifndef ABSTRACT_IMPLEMENTATION_HPP
#define ABSTRACT_IMPLEMENTATION_HPP
#include <vector>
#include "../common/Rectangle.h"

class AbstractImplementation {
public:
    AbstractImplementation(std::vector<Rectangle>&& rectangles) : rectangles(std::move(rectangles)) {}

    virtual void buildInternals() = 0;
    virtual int queryPoint(const Point& point) = 0;
protected:
    std::vector<Rectangle> rectangles;
};

#endif