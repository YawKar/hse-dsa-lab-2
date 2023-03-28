#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

class Point {
 public:
  long x;
  long y;

  Point(long x_ = 0, long y_ = 0) : x(x_), y(y_) {}
};

class Rectangle {
 public:
  Point leftDown;
  Point rightUp;

  Rectangle(long x1, long y1, long x2, long y2) : leftDown(x1, y1), rightUp(x2, y2) {}
  Rectangle(const Point& leftDown_, const Point& rightUp_) : leftDown(leftDown_), rightUp(rightUp_) {}
};

class AbstractImplementation {
 public:
  AbstractImplementation(std::vector<Rectangle>&& rectangles_) : rectangles(std::move(rectangles_)) {}

  virtual void buildInternals() = 0;
  virtual int queryPoint(const Point& point) = 0;

 protected:
  std::vector<Rectangle> rectangles;
};

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

PersistentSegmentTree::PersistentSegmentTree(std::vector<Rectangle>&& rectangles_)
    : AbstractImplementation(std::move(rectangles_)) {}

int PersistentSegmentTree::queryPoint(const Point& point) {
  if (point.x < this->zippedXs[0] || point.y < this->zippedYs[0]) {
    return 0;
  }
  std::size_t zippedXIdx = findUpperPos(this->zippedXs, point.x) - 1;
  std::size_t zippedYIdx = findUpperPos(this->zippedYs, point.y) - 1;
  std::shared_ptr<Node> targetRoot = this->roots[findUpperPos(this->zippedRootsXIdxs, zippedXIdx) - 1];
  return getTotalSum(targetRoot, 0, this->zippedYs.size(), zippedYIdx);
}

void PersistentSegmentTree::makeZippedCoordsFromRectangles() {
  for (const auto& rect : this->rectangles) {
    this->zippedXs.push_back(rect.leftDown.x);
    this->zippedXs.push_back(rect.rightUp.x);
    this->zippedXs.push_back(rect.rightUp.x + 1);
  }

  std::sort(this->zippedXs.begin(), this->zippedXs.end());
  this->zippedXs.erase(std::unique(this->zippedXs.begin(), this->zippedXs.end()), this->zippedXs.end());
  this->zippedXs.shrink_to_fit();

  for (const auto& rect : this->rectangles) {
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
  for (const auto& rect : this->rectangles) {
    events.emplace_back(findPos(this->zippedXs, rect.leftDown.x), 1, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
    events.emplace_back(findPos(this->zippedXs, rect.rightUp.x + 1), 0, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
  }

  this->rectangles.clear();
  this->rectangles.shrink_to_fit();

  std::sort(events.begin(), events.end(),
            [](const Event& e1, const Event& e2) { return e1.zippedXIdx < e2.zippedXIdx; });

  std::shared_ptr<Node> root(new Node);

  std::size_t prevZippedX = events[0].zippedXIdx;
  for (const auto& event : events) {
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

int main() {
  std::cin.tie(NULL);
  std::ios_base::sync_with_stdio(false);

  int n;
  std::cin >> n;

  std::vector<Rectangle> rectangles;
  rectangles.reserve(n);
  for (int i = 0; i < n; ++i) {
    int x1, y1, x2, y2;
    std::cin >> x1 >> y1 >> x2 >> y2;
    rectangles.emplace_back(x1, y1, x2, y2);
  }

  PersistentSegmentTree algorithm(std::move(rectangles));
  if (n > 0) algorithm.buildInternals();

  int m;
  std::cin >> m;
  for (int i = 0; i < m; ++i) {
    int x, y;
    std::cin >> x >> y;
    if (n > 0)
      std::cout << algorithm.queryPoint(Point(x, y)) << " ";
    else
      std::cout << 0 << ' ';
  }
}