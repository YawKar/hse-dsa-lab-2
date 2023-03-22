#include <algorithm>
#include "QubicMapBuilding.h"

QubicMapBuilding::QubicMapBuilding(std::vector<Rectangle>&& rectangles) 
: AbstractImplementation(std::move(rectangles)) {}

int QubicMapBuilding::findPos(std::vector<int>& items, int target) {
    return std::lower_bound(items.begin(), items.end(), target) - items.begin();
}

int QubicMapBuilding::queryPoint(const Point& point)
{
    Point zipped(findPos(this->zippedXs, point.x), findPos(this->zippedYs, point.y));
    if (zipped.x == this->zippedXs.size() || // `point` is to the right of the rightmost point of rectangles
        zipped.y == this->zippedYs.size() || // `point` is higher than the highest point of rectangles
        (zipped.x == 0 && point.x < this->zippedXs[0]) || // `point` is to the left of the leftmost point of rectangles
        (zipped.y == 0 && point.y < this->zippedYs[0]) // `point` is lower than the lowest point of rectangles
        ) {
        return 0; // `point` is out of the bounds
    }
    return this->map[zipped.x][zipped.y];
}

void QubicMapBuilding::buildInternals() {
    for (const auto& rect : this->rectangles) {
        this->zippedXs.push_back(rect.leftDown.x);
        this->zippedXs.push_back(rect.rightUp.x);
        this->zippedYs.push_back(rect.leftDown.y);
        this->zippedYs.push_back(rect.rightUp.y);
    }
    
    std::sort(this->zippedXs.begin(), this->zippedXs.end());
    std::sort(this->zippedYs.begin(), this->zippedYs.end());
    this->zippedXs.erase(std::unique(this->zippedXs.begin(), this->zippedXs.end()), this->zippedXs.end());
    this->zippedYs.erase(std::unique(this->zippedYs.begin(), this->zippedYs.end()), this->zippedYs.end());

    this->map.resize(zippedXs.size());
    for (int i = 0; i < zippedXs.size(); ++i) {
        this->map[i].resize(this->zippedYs.size());
    }

    for (const auto& rect : this->rectangles) {
        Point zippedLeftDown(findPos(this->zippedXs, rect.leftDown.x), findPos(this->zippedYs, rect.leftDown.y));
        Point zippedRightUp(findPos(this->zippedXs, rect.rightUp.x), findPos(this->zippedYs, rect.rightUp.y));
        for (int xIdx = zippedLeftDown.x; xIdx < zippedRightUp.x + 1; ++xIdx) {
            for (int yIdx = zippedLeftDown.y; yIdx < zippedRightUp.y + 1; ++yIdx) {
                ++this->map[xIdx][yIdx];
            }
        }
    }
}