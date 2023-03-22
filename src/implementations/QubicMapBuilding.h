#ifndef QUBIC_MAP_BUILDING_HPP
#define QUBIC_MAP_BUILDING_HPP

#include <vector>
#include "AbstractImplementation.h"

class QubicMapBuilding : AbstractImplementation {
public:
    QubicMapBuilding(std::vector<Rectangle>&& rectangles);
    void buildInternals();
    int queryPoint(const Point& point);
protected:
    std::vector<int> zippedXs;
    std::vector<int> zippedYs;
    std::vector<std::vector<int>> map;

    int findPos(std::vector<int>& items, int target);
};

#endif // QUBIC_MAP_BUILDING_HPP