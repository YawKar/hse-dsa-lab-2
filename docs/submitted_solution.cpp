#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

class Point {
 public:
  int x;
  int y;

  Point() : x(0), y(0) {}
  Point(int x_, int y_) : x(x_), y(y_) {}
};

class Rectangle {
 public:
  Point leftDown;
  Point rightUp;

  Rectangle(int x1, int y1, int x2, int y2)
      : leftDown(x1, y1), rightUp(x2, y2) {}
  Rectangle(Point& leftDown_, Point& rightUp_)
      : leftDown(leftDown_), rightUp(rightUp_) {}
};

class AbstractImplementation {
 public:
  AbstractImplementation(std::vector<Rectangle>&& rectangles)
      : rectangles(std::move(rectangles)) {}

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
    int zippedXIdx;
    bool isOpening;
    int zippedYIdxStart;
    int zippedYIdxEnd;
    Event(int zippedXIdx_, bool isOpening_, int zippedYIdxStart_,
          int zippedYIdxEnd_)
        : zippedXIdx(zippedXIdx_),
          isOpening(isOpening_),
          zippedYIdxStart(zippedYIdxStart_),
          zippedYIdxEnd(zippedYIdxEnd_) {}
  };

  std::vector<int> zippedXs;
  std::vector<int> zippedYs;
  std::vector<std::shared_ptr<Node>> roots;
  std::vector<int> zippedRootsXIdxs;

  int findPos(std::vector<int>& items, int target);
  int findUpperPos(std::vector<int>& items, int target);
  void makeZippedCoordsFromRectangles();
  std::shared_ptr<Node> addWithPersistence(std::shared_ptr<Node> root, int left,
                                           int right, int rangeStart,
                                           int rangeEnd, int value);
  int getTotalSum(std::shared_ptr<Node> root, int left, int right,
                  int targetIdx);
};

PersistentSegmentTree::PersistentSegmentTree(
    std::vector<Rectangle>&& rectangles)
    : AbstractImplementation(std::move(rectangles)) {}

int PersistentSegmentTree::findPos(std::vector<int>& items, int target) {
  return std::lower_bound(items.begin(), items.end(), target) - items.begin();
}

int PersistentSegmentTree::findUpperPos(std::vector<int>& items, int target) {
  return std::upper_bound(items.begin(), items.end(), target) - items.begin();
}

int PersistentSegmentTree::queryPoint(const Point& point) {
  if (point.x > this->zippedXs.back() ||   // `point` is to the right of the
                                           // rightmost point of rectangles
      point.y > this->zippedYs.back() ||   // `point` is higher than the highest
                                           // point of rectangles
      point.x < this->zippedXs.front() ||  // `point` is to the left of the
                                           // leftmost point of rectangles
      point.y <
          this->zippedYs
              .front()  // `point` is lower than the lowest point of rectangles
  ) {
    return 0;  // `point` is out of the bounds
  }
  int zippedXIdx = findUpperPos(this->zippedXs, point.x) - 1;
  int zippedYIdx = findUpperPos(this->zippedYs, point.y) - 1;
  std::shared_ptr<Node> targetRoot =
      this->roots[findUpperPos(this->zippedRootsXIdxs, zippedXIdx) - 1];
  return getTotalSum(targetRoot, 0, this->zippedYs.size(), zippedYIdx);
}

void PersistentSegmentTree::makeZippedCoordsFromRectangles() {
  for (const auto& rect : this->rectangles) {
    this->zippedXs.push_back(rect.leftDown.x);
    this->zippedXs.push_back(rect.rightUp.x);
    this->zippedXs.push_back(rect.rightUp.x + 1);
  }

  std::sort(this->zippedXs.begin(), this->zippedXs.end());
  this->zippedXs.erase(
      std::unique(this->zippedXs.begin(), this->zippedXs.end()),
      this->zippedXs.end());
  this->zippedXs.shrink_to_fit();

  for (const auto& rect : this->rectangles) {
    this->zippedYs.push_back(rect.leftDown.y);
    this->zippedYs.push_back(rect.rightUp.y);
    this->zippedYs.push_back(rect.rightUp.y + 1);
  }

  std::sort(this->zippedYs.begin(), this->zippedYs.end());
  this->zippedYs.erase(
      std::unique(this->zippedYs.begin(), this->zippedYs.end()),
      this->zippedYs.end());
  this->zippedYs.shrink_to_fit();
}

int PersistentSegmentTree::getTotalSum(std::shared_ptr<Node> root, int left,
                                       int right, int targetIdx) {
  if (right - left == 1) {
    return root->summ;
  }
  int middle = (left + right) / 2;
  if (targetIdx < middle) {
    if (root->left == nullptr) return root->summ;
    return root->summ + getTotalSum(root->left, left, middle, targetIdx);
  } else {
    if (root->right == nullptr) return root->summ;
    return root->summ + getTotalSum(root->right, middle, right, targetIdx);
  }
}

std::shared_ptr<PersistentSegmentTree::Node>
PersistentSegmentTree::addWithPersistence(std::shared_ptr<Node> root, int left,
                                          int right, int rangeStart,
                                          int rangeEnd, int value) {
  if (left >= rangeEnd || right <= rangeStart) {
    return root;
  }
  if (rangeStart <= left && right <= rangeEnd) {
    std::shared_ptr<Node> newRoot(new Node(*root));
    newRoot->summ += value;
    return newRoot;
  }
  int middle = (left + right) / 2;
  std::shared_ptr<Node> newRoot(new Node(*root));
  if (newRoot->left == nullptr) newRoot->left = std::shared_ptr<Node>(new Node);
  newRoot->left = addWithPersistence(newRoot->left, left, middle, rangeStart,
                                     rangeEnd, value);
  if (newRoot->right == nullptr)
    newRoot->right = std::shared_ptr<Node>(new Node);
  newRoot->right = addWithPersistence(newRoot->right, middle, right, rangeStart,
                                      rangeEnd, value);
  return newRoot;
}

void PersistentSegmentTree::buildInternals() {
  makeZippedCoordsFromRectangles();

  std::vector<Event> events;
  events.reserve(this->rectangles.size());
  for (const auto& rect : this->rectangles) {
    events.emplace_back(findPos(this->zippedXs, rect.leftDown.x), 1,
                        findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
    events.emplace_back(findPos(this->zippedXs, rect.rightUp.x + 1), 0,
                        findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
  }

  this->rectangles.clear();
  this->rectangles.shrink_to_fit();

  std::sort(events.begin(), events.end(), [](const Event& e1, const Event& e2) {
    return e1.zippedXIdx < e2.zippedXIdx;
  });

  std::shared_ptr<Node> root(new Node);

  int prevZippedX = events[0].zippedXIdx;
  for (std::size_t eventIdx = 0; eventIdx < events.size(); ++eventIdx) {
    if (events[eventIdx].zippedXIdx != prevZippedX) {
      this->roots.push_back(root);
      this->zippedRootsXIdxs.push_back(prevZippedX);
      prevZippedX = events[eventIdx].zippedXIdx;
    }
    root = addWithPersistence(
        root, 0, this->zippedYs.size(), events[eventIdx].zippedYIdxStart,
        events[eventIdx].zippedYIdxEnd, events[eventIdx].isOpening ? 1 : -1);
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