#include "PersistentSegmentTree.hpp"

#include <algorithm>

PersistentSegmentTree::PersistentSegmentTree(std::vector<Rectangle> &&rectangles_)
    : AbstractImplementation(std::move(rectangles_)) {}

int PersistentSegmentTree::queryPoint(const Point &point) {
  if (point.x < this->zippedXs[0] || point.y < this->zippedYs[0]) {
    return 0;
  }
  std::size_t zippedXIdx = findUpperPos(this->zippedXs, point.x) - 1;
  std::size_t zippedYIdx = findUpperPos(this->zippedYs, point.y) - 1;
  std::shared_ptr<Node> targetRoot = this->roots[findUpperPos(this->zippedRootsXIdxs, zippedXIdx) - 1];
  return getTotalSum(targetRoot, 0, this->zippedYs.size(), zippedYIdx);
}

void PersistentSegmentTree::makeZippedCoordsFromRectangles() {
  for (const auto &rect : this->rectangles) {
    this->zippedXs.push_back(rect.leftDown.x);
    this->zippedXs.push_back(rect.rightUp.x);
    this->zippedXs.push_back(rect.rightUp.x + 1);
  }

  std::sort(this->zippedXs.begin(), this->zippedXs.end());
  this->zippedXs.erase(std::unique(this->zippedXs.begin(), this->zippedXs.end()), this->zippedXs.end());
  this->zippedXs.shrink_to_fit();

  for (const auto &rect : this->rectangles) {
    this->zippedYs.push_back(rect.leftDown.y);
    this->zippedYs.push_back(rect.rightUp.y);
    this->zippedYs.push_back(rect.rightUp.y + 1);
  }

  std::sort(this->zippedYs.begin(), this->zippedYs.end());
  this->zippedYs.erase(std::unique(this->zippedYs.begin(), this->zippedYs.end()), this->zippedYs.end());
  this->zippedYs.shrink_to_fit();
}

int PersistentSegmentTree::getTotalSum(std::shared_ptr<Node> root, std::size_t left, std::size_t right,
                                       std::size_t targetIdx) {
  if (right - left == 1) {
    return root->summ;
  }
  std::size_t middle = (left + right) / 2;
  if (targetIdx < middle) {
    if (root->left == nullptr) return root->summ;
    return root->summ + getTotalSum(root->left, left, middle, targetIdx);
  } else {
    if (root->right == nullptr) return root->summ;
    return root->summ + getTotalSum(root->right, middle, right, targetIdx);
  }
}

std::shared_ptr<PersistentSegmentTree::Node> PersistentSegmentTree::addWithPersistence(
    std::shared_ptr<Node> root, std::size_t left, std::size_t right, std::size_t rangeStart, std::size_t rangeEnd,
    int value) {
  if (left >= rangeEnd || right <= rangeStart) {
    return root;
  }
  if (rangeStart <= left && right <= rangeEnd) {
    std::shared_ptr<Node> newRoot(new Node(*root));
    newRoot->summ += value;
    return newRoot;
  }
  std::size_t middle = (left + right) / 2;
  std::shared_ptr<Node> newRoot(new Node(*root));
  if (newRoot->left == nullptr) newRoot->left = std::shared_ptr<Node>(new Node);
  newRoot->left = addWithPersistence(newRoot->left, left, middle, rangeStart, rangeEnd, value);
  if (newRoot->right == nullptr) newRoot->right = std::shared_ptr<Node>(new Node);
  newRoot->right = addWithPersistence(newRoot->right, middle, right, rangeStart, rangeEnd, value);
  return newRoot;
}

void PersistentSegmentTree::buildInternals() {
  makeZippedCoordsFromRectangles();

  std::vector<Event> events;
  events.reserve(2 * this->rectangles.size());
  for (const auto &rect : this->rectangles) {
    events.emplace_back(findPos(this->zippedXs, rect.leftDown.x), 1, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
    events.emplace_back(findPos(this->zippedXs, rect.rightUp.x + 1), 0, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
  }

  this->rectangles.clear();
  this->rectangles.shrink_to_fit();

  std::sort(events.begin(), events.end(),
            [](const Event &e1, const Event &e2) { return e1.zippedXIdx < e2.zippedXIdx; });

  std::shared_ptr<Node> root(new Node);

  std::size_t prevZippedX = events[0].zippedXIdx;
  for (const auto &event : events) {
    if (event.zippedXIdx != prevZippedX) {
      this->roots.push_back(root);
      this->zippedRootsXIdxs.push_back(prevZippedX);
      prevZippedX = event.zippedXIdx;
    }
    root = addWithPersistence(root, 0, this->zippedYs.size(), event.zippedYIdxStart, event.zippedYIdxEnd,
                              event.isOpening ? 1 : -1);
  }
  this->zippedRootsXIdxs.push_back(prevZippedX);
  this->roots.push_back(root);
}