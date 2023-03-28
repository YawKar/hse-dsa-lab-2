#ifndef QUBIC_MAP_BUILDING_HPP
#define QUBIC_MAP_BUILDING_HPP

#include <vector>

#include "AbstractImplementation.hpp"

class QubicMapBuilding : AbstractImplementation {
 public:
  QubicMapBuilding(std::vector<Rectangle>&& rectangles);
  void buildInternals();
  int queryPoint(const Point& point);

 protected:
  std::vector<long> zippedXs;
  std::vector<long> zippedYs;
  std::vector<std::vector<int>> map;

  std::size_t findPos(std::vector<long>& items, long target);
};

#endif  // QUBIC_MAP_BUILDING_HPP