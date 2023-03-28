#include "QubicMapBuilding.hpp"

#include <algorithm>

QubicMapBuilding::QubicMapBuilding(std::vector<Rectangle> &&rectangles_)
    : AbstractImplementation(std::move(rectangles_)) {}

std::size_t QubicMapBuilding::findPos(std::vector<long> &items, long target) {
  return std::upper_bound(items.begin(), items.end(), target) - items.begin() - 1;
}

int QubicMapBuilding::queryPoint(const Point &point) {
  if (point.x < this->zippedXs[0] || point.y < this->zippedYs[0]) {
    return 0;
  }
  std::size_t zippedX = findPos(this->zippedXs, point.x);
  std::size_t zippedY = findPos(this->zippedYs, point.y);
  return this->map[zippedX][zippedY];
}

void QubicMapBuilding::buildInternals() {
  for (const auto &rect : this->rectangles) {
    this->zippedXs.push_back(rect.leftDown.x);
    this->zippedXs.push_back(rect.rightUp.x);
    this->zippedXs.push_back(rect.rightUp.x + 1);
    this->zippedYs.push_back(rect.leftDown.y);
    this->zippedYs.push_back(rect.rightUp.y);
    this->zippedYs.push_back(rect.rightUp.y + 1);
  }

  std::sort(this->zippedXs.begin(), this->zippedXs.end());
  std::sort(this->zippedYs.begin(), this->zippedYs.end());
  this->zippedXs.erase(std::unique(this->zippedXs.begin(), this->zippedXs.end()), this->zippedXs.end());
  this->zippedYs.erase(std::unique(this->zippedYs.begin(), this->zippedYs.end()), this->zippedYs.end());

  this->map.resize(zippedXs.size());
  for (std::size_t i = 0; i < zippedXs.size(); ++i) {
    this->map[i].resize(this->zippedYs.size());
  }

  for (const auto &rect : this->rectangles) {
    Point zippedLeftDown(findPos(this->zippedXs, rect.leftDown.x), findPos(this->zippedYs, rect.leftDown.y));
    Point zippedRightUp(findPos(this->zippedXs, rect.rightUp.x), findPos(this->zippedYs, rect.rightUp.y));
    for (long xIdx = zippedLeftDown.x; xIdx < zippedRightUp.x + 1; ++xIdx) {
      for (long yIdx = zippedLeftDown.y; yIdx < zippedRightUp.y + 1; ++yIdx) {
        ++this->map[xIdx][yIdx];
      }
    }
  }
}