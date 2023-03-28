#include "NaiveRectangleEnumeration.hpp"

NaiveRectangleEnumeration::NaiveRectangleEnumeration(std::vector<Rectangle> &&rectangles_)
    : AbstractImplementation(std::move(rectangles_)) {}

int NaiveRectangleEnumeration::queryPoint(const Point &point) {
  int answer = 0;
  for (const auto &rectangle : this->rectangles) {
    if (rectangle.leftDown.x <= point.x && point.x <= rectangle.rightUp.x && rectangle.leftDown.y <= point.y &&
        point.y <= rectangle.rightUp.y) {
      ++answer;
    }
  }
  return answer;
}

void NaiveRectangleEnumeration::buildInternals() {}