#ifndef PERSISTENT_SEGMENT_TREE_HPP
#define PERSISTENT_SEGMENT_TREE_HPP

#include <algorithm>
#include <memory>
#include <vector>

#include "AbstractImplementation.hpp"

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
    std::size_t zippedXIdx;
    bool isOpening;
    std::size_t zippedYIdxStart;
    std::size_t zippedYIdxEnd;
    Event(std::size_t zippedXIdx_, bool isOpening_, std::size_t zippedYIdxStart_, std::size_t zippedYIdxEnd_)
        : zippedXIdx(zippedXIdx_),
          isOpening(isOpening_),
          zippedYIdxStart(zippedYIdxStart_),
          zippedYIdxEnd(zippedYIdxEnd_) {}
  };

  std::vector<long> zippedXs;
  std::vector<long> zippedYs;
  std::vector<std::shared_ptr<Node>> roots;
  std::vector<std::size_t> zippedRootsXIdxs;

  template <class T>
  std::size_t findPos(std::vector<T>& items, T target) {
    return std::lower_bound(items.begin(), items.end(), target) - items.begin();
  }
  template <class T>
  std::size_t findUpperPos(std::vector<T>& items, T target) {
    return std::upper_bound(items.begin(), items.end(), target) - items.begin();
  }
  void makeZippedCoordsFromRectangles();
  std::shared_ptr<Node> addWithPersistence(std::shared_ptr<Node> root, std::size_t left, std::size_t right,
                                           std::size_t rangeStart, std::size_t rangeEnd, int value);
  int getTotalSum(std::shared_ptr<Node> root, std::size_t left, std::size_t right, std::size_t targetIdx);
};

#endif  // PERSISTENT_SEGMENT_TREE_HPP