#ifndef PERSISTENT_SEGMENT_TREE_HPP
#define PERSISTENT_SEGMENT_TREE_HPP

#include <vector>
#include <memory>
#include "AbstractImplementation.h"

class PersistentSegmentTree : AbstractImplementation {
public:
    PersistentSegmentTree(std::vector<Rectangle>&& rectangles);
    void buildInternals();
    int queryPoint(const Point& point);
protected:
    struct Node {
        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;
        int summ = 0;
    };

    struct Event {
        int zippedXIdx;
        bool isOpening;
        int zippedYIdxStart;
        int zippedYIdxEnd;
        Event(int zippedXIdx_, bool isOpening_, int zippedYIdxStart_, int zippedYIdxEnd_) 
        : zippedXIdx(zippedXIdx_), isOpening(isOpening_), zippedYIdxStart(zippedYIdxStart_), zippedYIdxEnd(zippedYIdxEnd_) {}
    };

    std::vector<int> zippedXs;
    std::vector<int> zippedYs;
    std::vector<std::shared_ptr<Node>> roots;
    std::vector<int> zippedRootsXIdxs;

    int findPos(std::vector<int>& items, int target);
    int findUpperPos(std::vector<int>& items, int target);
    void makeZippedCoordsFromRectangles();
    std::shared_ptr<Node> addWithPersistence(std::shared_ptr<Node> root, int left, int right, int rangeStart, int rangeEnd, int value);
    int getTotalSum(std::shared_ptr<Node> root, int left, int right, int targetIdx);
};

#endif // PERSISTENT_SEGMENT_TREE_HPP