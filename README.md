# Performance Comparison Lab №2
This file is available in languages: [**English**✅](README.md), [Russian](README_RU.md)

The objective of this laboratory work is to compare the performance of multiple algorithms designed to respond in real-time to queries regarding the number of rectangles that a given point (x, y) belongs to.

# Table of contents
1. [Problem Statement and Constraints](#problem-statement-and-constraints)
    - [Problem Statement](#problem-statement)
    - [Constraints](#constraints)
        - [Rectangles](#rectangles)
        - [Points](#points)
2. [Utilized Software Toolset](#utilized-software-toolset)
    - [C++](#c)
    - [Conan 2.0](#conan-20)
    - [CMake](#cmake)
    - [Google Benchmark](#google-benchmark)
3. [Test Case Generation](#test-case-generation)
4. [First Approach: Naive Brute Force Algorithm](#first-approach-naive-brute-force-algorithm)
5. [Second Approach: Compressed Coordinate Map Construction](#second-approach-compressed-coordinate-map-construction)
6. [Third Approach: Lazy Persistent Segment Tree on Compressed Coordinates](#third-approach-lazy-persistent-segment-tree-on-compressed-coordinates)
    - [Lazy Persistent Segment Tree](#lazy-persistent-segment-tree)
    - [Key idea: event-driven updates with preservation of the history of tree state](#key-idea-event-driven-updates-with-preservation-of-the-history-of-tree-state)
    - [Coordinate compression](#coordinate-compression)
    - [Events construction](#events-construction)
    - [Querying points](#querying-points)
7. [Benchmark Graphs, Comparison, and Thoughts](#benchmark-graphs-comparison-and-thoughts)
8. [Benchmarks](#benchmarks)
9. [Run it yourself!](#run-it-yourself)
    - [Installation](#installation)
        - [How to install CMake](#how-to-install-cmake)
        - [How to install Conan](#how-to-install-conan)
        - [How to install dependencies via Conan](#how-to-install-dependencies-via-conan)
    - [Running](#running)
        - [Initial setup and generation of toolchain files](#initial-setup-and-generation-of-toolchain-files)
        - [Building and running the executable](#building-and-running-the-executable)
        - [Supported command line arguments](#supported-command-line-arguments)

# Problem Statement and Constraints
## Problem statement
Given rectangles on a two-dimensional plane, the task is to respond as quickly as possible to the query "How many rectangles does the point (x,y) belong to?".

## Constraints
### Rectangles
Each rectangle is defined by a pair of coordinates of its bottom-left and top-right corners: `x1 y1 x2 y2`.

Where `x1`, `y1`, `x2`, `y2` satisfy the following conditions:
- `x1, y1, x2, y2` are integers
- `0 <= x1, y1, x2, y2 <= 1e9`
- `x1 < x2`
- `y1 < y2`

C++ `Rectangle` class implementation:
```cpp
class Rectangle {
 public:
  Point leftDown;
  Point rightUp;

  Rectangle(long x1, long y1, long x2, long y2) : leftDown(x1, y1), rightUp(x2, y2) {}
  Rectangle(const Point& leftDown_, const Point& rightUp_) : leftDown(leftDown_), rightUp(rightUp_) {}
};
```

### Points
Each point is defined by a pair of its coordinates: `x y`.

Where `x` and `y` satisfy the following conditions:
- `x, y` are integers
- `-1e9 <= x, y <= 1e9`

C++ `Point` class implementation:
```cpp
class Point {
 public:
  long x;
  long y;

  Point(long x_ = 0, long y_ = 0) : x(x_), y(y_) {}
};
```

# Utilized Software Toolset
## C++
To conduct the laboratory work, the compiled language C++ was chosen. 
The C++ language was used to implement the algorithms. The version of the compiler used was GCC 11.3.0. The compilation flags were set to `-O3` to enable maximum optimization. The program was run on a machine with the following specifications:
| Property       | Value                                     |
|:---------------|------------------------------------------:|
| Architecture   | x86_64                                    |
| CPU op-mode(s) | 32-bit, 64-bit                            |
| Address sizes  | 39 bits physical, 48 bits virtual         |
| Byte Order     | Little Endian                             |
| CPU(s)         | 12                                        |
| Vendor ID      | GenuineIntel                              |
| Model name     | Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz |
| CPU max MHz    | 5000,0000                                 |
| CPU min MHz    | 800,0000                                  |
| Virtualization | VT-x                                      |
| Cache (L1d)    | 192 KiB (6 instances)                     |
| Cache (L1i)    | 192 KiB (6 instances)                     |
| Cache (L2)     | 1,5 MiB (6 instances)                     |
| Cache (L3)     | 12 MiB (1 instance)                       |

An alternative in the form of Java was also considered, but it was decided to abandon it due to the uncertainty and noise that various runtime optimizations (warming up) and garbage collection introduce into measurements. Configuring the JVM for benchmarking is quite difficult and requires a proper level of expertise, which I'm afraid I don't have.

## Conan 2.0
For the convenience of development, source distribution, and library dependency management, I decided to use the modern package manager Conan 2.0. Thanks to it, to connect all dependencies and run the project, it is necessary to execute only a limited set of commands, which, after initial configuration of Conan, will be reduced to 2 commands: `cmake --build .`, `./lab_benchmark`.

## CMake
The project's build system is based on the industry standard - CMake. The .cmake files generated by Conan are used for building.

## Google Benchmark
I'm using the [Google Benchmark library](https://github.com/google/benchmark) for benchmarks, which I've included via Conan. It has a wide API and is very easy to use.

# Test Case Generation
To generate test cases for the problem, it was suggested to generate rectangles according to a certain rule:
- The recommended approach for generating a test set of rectangles is to use a set of nested rectangles with coordinates that have a step size greater than 1, such as `{(10*i, 10*i), (10*(2*N-i), 10*(2*N-i))}`.

As for generating points, it was advised to generate them somehow uniformly across the entire plane.

I decided to encapsulate test generation logic inside of the `TestCaseGenerator` class:
```cpp
class TestCaseGenerator {
 public:
  static std::vector<Rectangle> generateRecommendedRectangles(int rectangles);
  static std::vector<Point> generateUniformlyDistributedPoints(int numberOfPoints, int minX, int maxX, int minY,
                                                               int maxY, int xSeed = std::random_device()(),
                                                               int ySeed = std::random_device()());
};
```
Implementation of `generateRecommendedRectangles`:
```cpp
std::vector<Rectangle> TestCaseGenerator::generateRecommendedRectangles(int numberOfRectangles) {
  std::vector<Rectangle> rects;
  rects.reserve(numberOfRectangles);
  for (int i = 0; i < numberOfRectangles; ++i) {
    rects.emplace_back(10 * i, 10 * i, 10 * (2 * numberOfRectangles - i), 10 * (2 * numberOfRectangles - i));
  }
  return rects;
}
```
Implementation of `generateUniformlyDistributedPoints`:
```cpp
std::vector<Point> TestCaseGenerator::generateUniformlyDistributedPoints(int numberOfPoints, int minX, int maxX,
                                                                         int minY, int maxY, int xSeed, int ySeed) {
  std::mt19937 xEngine(xSeed);
  std::mt19937 yEngine(ySeed);
  std::uniform_int_distribution<int> xDist(minX, maxX);
  std::uniform_int_distribution<int> yDist(minY, maxY);

  std::vector<Point> points;
  points.reserve(numberOfPoints);
  for (int i = 0; i < numberOfPoints; ++i) {
    points.emplace_back(xDist(xEngine), yDist(yEngine));
  }
  return points;
}
```

# First Approach: Naive Brute Force Algorithm
The first approach does not involve any data preprocessing. All rectangles are stored in a vector "as is". To obtain the answer for a specific point, we iterate over all rectangles and check two conditions: whether the given point is within this rectangle on both axes.

As I've mentioned before, there is no data preprocessing:
```cpp
void NaiveRectangleEnumeration::buildInternals() {}
```
The brute force implementation of query:
```cpp
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
```

# Second Approach: Compressed Coordinate Map Construction
The second approach involves compressing the coordinates of all the rectangle points along both axes. We create a two-dimensional array over the compressed points, initially filled with zeros. Then, for each rectangle, we add `1` to each cell of the two-dimensional array that is located under the rectangle. As a result, we obtain a "height map", where the value of `map[i][j]` equals the number of rectangles that cover the two-dimensional compressed subspace at the point `(i, j)`.

To find the answer for a given point `(x, y)`, we will search for the indexes of the compressed coordinates along both axes. Specifically, we will look for **the largest compressed coordinate that is less than or equal to** the corresponding coordinate of the point. Once we find the compressed coordinates of the point, we check whether the point is inside the working area. If it is, we simply return the value of `map[zipped.x][zipped.y]`. Otherwise, the point is outside the bounds of the working area, and the answer is `0`.

Implementation of coordinate compression and map building:
```cpp
void QubicMapBuilding::buildInternals() {
  // Here we accumulate all x-coords and y-coords. 
  // Moreover, for each rectangle we also accumulate coords of the point that is 1 unit higher 
  // and 1 unit to the right than the rightmost upper point of the rectangle. 
  for (const auto &rect : this->rectangles) {
    this->zippedXs.push_back(rect.leftDown.x);
    this->zippedXs.push_back(rect.rightUp.x);
    this->zippedXs.push_back(rect.rightUp.x + 1);
    this->zippedYs.push_back(rect.leftDown.y);
    this->zippedYs.push_back(rect.rightUp.y);
    this->zippedYs.push_back(rect.rightUp.y + 1);
  }

  // Next, we need to sort these coordinates in order to effectively delete duplicates
  std::sort(this->zippedXs.begin(), this->zippedXs.end());
  std::sort(this->zippedYs.begin(), this->zippedYs.end());
  this->zippedXs.erase(std::unique(this->zippedXs.begin(), this->zippedXs.end()), this->zippedXs.end());
  this->zippedYs.erase(std::unique(this->zippedYs.begin(), this->zippedYs.end()), this->zippedYs.end());

  // Allocate two-dimensional map with '0' as default value
  this->map.resize(zippedXs.size());
  for (std::size_t i = 0; i < zippedXs.size(); ++i) {
    this->map[i].resize(this->zippedYs.size());
  }

  // This takes O(n^3) operations to build the final "height map".
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
```
Implementation of query:
```cpp
int QubicMapBuilding::queryPoint(const Point &point) {
  // We don't need to check top and right sides because 
  // if the point is higher than the highest point of the highest rectangle
  // or the point is to the right of the rightmost point of the rightmost rectangle
  // this point's zipped coordinates will point to the free space that was composed
  // thanks to our additional coordinates that we've saved in `QubicMapBuilding::buildInternals()`.
  if (point.x < this->zippedXs[0] || point.y < this->zippedYs[0]) {
    return 0;
  }
  std::size_t zippedX = findPos(this->zippedXs, point.x);
  std::size_t zippedY = findPos(this->zippedYs, point.y);
  return this->map[zippedX][zippedY];
}
```
Implementation of the util method `findPos(..)` that performs search for compressed indexes:
```cpp
std::size_t QubicMapBuilding::findPos(std::vector<long> &items, long target) {
  return std::upper_bound(items.begin(), items.end(), target) - items.begin() - 1;
}
```

# Third Approach: Lazy Persistent Segment Tree on Compressed Coordinates
The third approach requires prior knowledge of lazy persistent segment trees. After reviewing the definition and implementation of the "Lazy Persistent Segment Tree", we will be ready to proceed with the final solution.

## Lazy Persistent Segment Tree
The solution requires a lazy persistent segment tree, which stores a number in each node and allows for fast addition/subtraction of a number on random subsegments `[l; r)`.

Let's start by describing the structure of a node. The node will contain three fields: a pointer to the left child, a pointer to the right child, and the accumulated sum in the node. By default, the pointers are `nullptr`, and the sum is `0`.
```cpp
struct Node {
  std::shared_ptr<Node> left = nullptr;
  std::shared_ptr<Node> right = nullptr;
  int summ = 0;
};
```

We implement the operation of adding/subtracting a number on a subsegment through lazy propagation until we descend to a node that corresponds to a subsegment covered by the target subsegment (`// Condition 2`). In this case, we create a new copy of the node to maintain persistence and increase/decrease the accumulated sum of the copy by the given number.

If we have descended to a node that corresponds to a subsegment not covered by the target subsegment (`// Condition 1`), then in this case, we simply return a pointer to the current node, without applying any changes.

Otherwise, we recursively descend into both children of the current root, creating these children if they did not exist. **This is the "laziness" of this segment tree, it builds itself only as needed**.

In essence, this function returns new root node that corresponds to new 'state' of the segment tree.
```cpp
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
```

Now, we can use root of the tree to collect information about how many subsegments cover the point `x`. In order to perform that we need to descend to the lowest hanging node that is exist and is covering the point `x`. We can do it using binary search:
```cpp
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
```
If the current root corresponds to a subsegment of length `1`, then we simply return the accumulated sum. Otherwise, firstly, we calculate the middle point of the `[left; right)` subsegment, and depending on the location of the `targetIdx`, we recursively dive into the left or right branch if the branch exists. If it doesn't exist, we return the root's accumulated sum.

## Key idea: event-driven updates with preservation of the history of tree state
The key idea of the solution is to update the tree for each column (each compressed x-coordinate) in the order of increasing columns from left to right, preserving the new roots of the persistent segment tree and their corresponding compressed coordinates. Then, when a query point arrives, we use binary search to find the index of the compressed coordinate along the x-axis, retrieve the tree root that was saved at the time of processing events for that x-coordinate, and perform a standard one-dimensional query along the y-coordinate, obtaining the answer in `O(log(n))`.

## Coordinate compression
Coordinate compression is implemented in a similar way as it was in the 2nd approach. The only difference is that we add not just rectangle's corners coordinates but also coordinates of the point that is 1 unit higher and 1 unit to the right of the right upper corner of the rectangle. We will need these coordinates for correct compressing of point coordinates and assignment of events' coordinates as well as for one-dimensional (along the y-coordinate) querying on segment tree.
```cpp
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
```

## Events construction
Every rectangle has 4 sides. We need to create a sequence of sorted events each of which represents left or right side of a rectangle.

Let's consider the structure of an event:
```cpp
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
```
There `zippedXIdx` is the compressed x-coordinate of the left or right side of some rectangle. `isOpening` is a marker representing whether it is a left or right side (left if `true`, `false` otherwise, because we process events from left to right along the x-axis and the left side 'opens' the rectangle, whereas the right side `closes` it). `zippedYIdxStart` is the compressed y-coordinate of the left bottom corner of the rectangle, and also it is the beginning of the subsegment that corresponds to that rectangle's side. `zippedYIdxEnd` is the compressed y-coordinate of the point 1 unit higher and 1 unit to the right of the right top corner of the rectangle, and also it is the exclusive end of the subsegment that corresponds to that rectangle's side.

Now checkout the implementation of events creation:
```cpp
void PersistentSegmentTree::buildInternals() {
  // Compress rectangles' coordinates and save them to `this->zippedXs` and `this->zippedYs`.
  makeZippedCoordsFromRectangles();

  std::vector<Event> events;
  // Reserve 2*N memory so vector won't spend time on reallocating.
  events.reserve(this->rectangles.size());
  
  for (const auto &rect : this->rectangles) {
    // Create opening event that will take its place when sweepline reaches zipped(rect.leftDown.x) coordinate.
    // Essentially, this event will add 1 on the subsegment `[zipped(rect.leftDown.y), zipped(rect.rightUp.y + 1))`.
    events.emplace_back(findPos(this->zippedXs, rect.leftDown.x), 1, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
    // Create closing event that will take its place when sweepline reaches zipped(rect.rightUp.x + 1) coordinate.
    // Essentially, this event will subtract 1 on the subsegment `[zipped(rect.leftDown.y), zipped(rect.rightUp.y + 1))`.
    events.emplace_back(findPos(this->zippedXs, rect.rightUp.x + 1), 0, findPos(this->zippedYs, rect.leftDown.y),
                        findPos(this->zippedYs, rect.rightUp.y + 1));
  }

  // Free the memory allocated for storing rectangles. If you don't do it, you may face MLE (Memory Limit Exceeded error).
  // Or may not, to be honest, I didn't check it.
  this->rectangles.clear();
  this->rectangles.shrink_to_fit();

  // Sort events in ascending order according to their zipped x-coordinates
  std::sort(events.begin(), events.end(),
            [](const Event &e1, const Event &e2) { return e1.zippedXIdx < e2.zippedXIdx; });

  // Create empty (for a while) root
  std::shared_ptr<Node> root(new Node);

  std::size_t prevZippedX = events[0].zippedXIdx;
  for (const auto &event : events) {
    // If current event is not on the same zipped x-coordinate, we need to save current root
    // in order to be able to timewarp ourselves to that moment and query the point using
    // this state of the tree.
    if (event.zippedXIdx != prevZippedX) {
      this->roots.push_back(root);
      this->zippedRootsXIdxs.push_back(prevZippedX);
      // Update the `currently working on` zipped x-coordinate
      prevZippedX = event.zippedXIdx;
    }
    // Perform the 'event' (after event finish, we will get new root with applied changes)
    // on the current root. Initial subsegment of the whole tree is `[0, this->zippedYs.size())`.
    // Targeted subsegment is `[event.zippedYIdxStart, event.zippedYIdxEnd).
    // If the `event` is `addition` than we set +1, otherwise -1.
    root = addWithPersistence(root, 0, this->zippedYs.size(), event.zippedYIdxStart, event.zippedYIdxEnd,
                              event.isOpening ? 1 : -1);
  }
  // Save the last root's zipped x-coordinate.
  this->zippedRootsXIdxs.push_back(prevZippedX);
  // Preserve the last updated root (Invariant is: all of this root's nodes contain zero as their sum).
  this->roots.push_back(root);
}
```

After that we will have a set of persisted roots of the segment tree each with the corresponding compressed x-coordinate. It's time to review the final method that will query points in `O(log(N))`.

## Querying points
Initially, we need to check whether the point is even within the working area, if it isn't we can simply return 0 as it guarantees that the point is out of bounds of any rectangle.

If the point is within the working area, we find the corresponding compressed coordinates, which will be the maximum coordinates that are also less than or equal to those of the point. Then, in the array of saved compressed x-coordinates of the roots of the persistent segment tree, we find the root with the maximum index of compressed x-coordinate that is also less than or equal to that of the compressed input point. Once we obtain the root, we simply perform a one-dimensional query on it to obtain the accumulated sum over all subsegments that include the compressed y-coordinate of the point.
```cpp
int PersistentSegmentTree::queryPoint(const Point &point) {
  // This part is almost the same as in QubicMapBuilding.
  if (point.x < this->zippedXs[0] || point.y < this->zippedYs[0]) {
    return 0;
  }
  std::size_t zippedXIdx = findUpperPos(this->zippedXs, point.x) - 1;
  std::size_t zippedYIdx = findUpperPos(this->zippedYs, point.y) - 1;
  // Here we need to find the state of the tree in which it was in the moment of
  // processing zippedXIdx.
  std::shared_ptr<Node> targetRoot = this->roots[findUpperPos(this->zippedRootsXIdxs, zippedXIdx) - 1];
  // Simply making one-dimensional query to the tree.
  return getTotalSum(targetRoot, 0, this->zippedYs.size(), zippedYIdx);
}
```
Implementation of the util method `findUpperPos(..)` that performs search for compressed indexes that are `>`:
```cpp
template <class T>
std::size_t findUpperPos(std::vector<T>& items, T target) {
  return std::upper_bound(items.begin(), items.end(), target) - items.begin();
}
```
Implementation of the util method `findPos(..)` that performs search for compressed indexes that are `>=`:
```cpp
template <class T>
std::size_t findPos(std::vector<T>& items, T target) {
  return std::lower_bound(items.begin(), items.end(), target) - items.begin();
}
```

# Benchmark Graphs, Comparison, and Thoughts
Each of the algorithms performs some preprocessing (except for the first algorithm) with its own asymptotic complexity. At the same time, each of the algorithms responds to queries in its own way. Therefore, I decided to divide the measurements into:
- Time complexity of building the internal structure of the algorithm
- Average time complexity per one query to the constructed data structure

## Time complexity of building the internal structure of the algorithm
![Time complexity of building a data structure (real scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20building%20a%20data%20structure%20(real%20scale).png?raw=true)

As we can see above, `QubicMapBuilding` (2nd approach) is the slowest in the building phase. As I've mentioned in "[Second Approach: Compressed Coordinate Map Construction](#second-approach-compressed-coordinate-map-construction)" it takes `O(n^3)` to build the 2D map on compressed coordinates. Therefore it's not a surprise that it takes 48168000000 nanoseconds (48.168 seconds) to build map for `n = 5000` on my machine. As for the other two algorithms, they're incredibly faster. Because `NaiveRectangleEnumeration` (1st approach) does not have any building phase at all and `PersistentSegmentTree` builds itself in `O(n log(n))`.

![Time complexity of building a data structure (semi-logarithmic scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20building%20a%20data%20structure%20(semi-logarithmic%20scale).png?raw=true)

The graph on a semi-logarithmic scale does not give us anything new except for the visual difference between `O(1)` building phase of `NaiveRectangleEnumeration` and `O(n log(n))` of that of `PersistentSegmentTree` which is obvious even without visual reinforcement.

![Time complexity of building a data structure (logarithmic scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20building%20a%20data%20structure%20(logarithmic%20scale).png?raw=true)

Finally, the graph on the logarithmic scale gives us an interesting insight:
- `PersistentSegmentTree`'s building phase takes more time than that of `QubicMapBuilding` until number of rectangles reaches ~150. I believe that such a difference may be conditioned by constants, which are big in case of `PersistentSegmentTree` and tiny in case of `QubicMapBuilding`. 

## Average time complexity per one query to the constructed data structure
![Time complexity of one query (i.e. request) (real scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20one%20query%20(i.e.%20request)%20(real%20scale).png?raw=true)

From the graph above we can make a clear conclusion that `NaiveRectangleEnumeration` has `O(n)` time complexity for querying, whilst others' time complexities are `O(log(n))` or around it (and on that picture it actually looks like they have `O(1)` time complexity but they don't).

![Time complexity of one query (i.e. request) (semi-logarithmic scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20one%20query%20(i.e.%20request)%20(semi-logarithmic%20scale).png?raw=true)

The graph on a semi-logarithmic scale makes the difference between `QubicMapBuilding` and `PersistentSegmentTree` more distinguishable for us to see. 

From that graph we can state that:
- Constant of `PersistentSegmentTree` is higher than that of `QubicMapBuilding`, namely, twice of that of `QubicMapBuilding`. (I checked by data and it's indeed `~2`)

![Time complexity of one query (i.e. request) (logarithmic scale).png](https://github.com/YawKar/hse-dsa-lab-2/blob/main/docs/png/Time%20complexity%20of%20one%20query%20(i.e.%20request)%20(logarithmic%20scale).png?raw=true)

Lastly, the graph on logarithmic scale appears to show us that:
- Until the number of rectangles reaches 100, all three algorithms work equally efficient.

# Benchmarks
All benchmarks are in `main.cpp` file, you can go and check them out manually or read this short description.

Base structure of all benchmarks that measure building phase looks like that:
```cpp
static void BM_Building_NaiveRectangleEnumeration(benchmark::State &state) {
  // Starts the timer for each iteration (i.e. one distinct measurement)
  for (auto _ : state) {
    // Pause timing for test generation process to prevent contamination of results.
    state.PauseTiming();
    // Generate rectangles for the test
    auto rectangles = getRectangles(state.range(0));
    // Move input data inside the algorithm preset (without starting building the internals)
    auto algorithm = NaiveRectangleEnumeration(std::move(rectangles));
    // Resume timing to count the `buildInternals()` time consumption
    state.ResumeTiming();

    // The actual `building phase` being measured
    algorithm.buildInternals();
  }
}
```
Base structure of all benchmarks that measure average per query complexity looks like that:
```cpp
static void BM_PerRequest_NaiveRectangleEnumeration(benchmark::State &state) {
  // Generate rectangles for the test
  auto rectangles = getRectangles(state.range(0));
  // Generate points for the test
  auto points = getPoints(state.range(0));
  // Set the current point index (we will measure average query time complexity looping through 'points')
  std::size_t currentIdx = 0;
  // Move input data inside the algorithm preset (without starting building the internals)
  auto algorithm = NaiveRectangleEnumeration(std::move(rectangles));
  // Build the internals
  algorithm.buildInternals();

  // Starts the timer for each iteration (i.e. one distinct measurement)
  for (auto _ : state) {
    // Query the `currentIdx`th point
    algorithm.queryPoint(points[currentIdx++]);

    // Stop measurements to prevent %= from creating noises in results (probably can be removed)
    state.PauseTiming();
    currentIdx %= points.size();
    // Resume measurements
    state.ResumeTiming();
  }
}
```

# Run it yourself!
## Installation
### How to install CMake
1. You should check out [official ways to insall CMake, depending on your platform](https://cmake.org/install/)
2. After you installed it, check whether it works with \``cmake --version`\`, it should print something like \``cmake version x.xx.x`\` and etc

### How to install Conan
1. Check out [official ways to install Conan in your system](https://docs.conan.io/2/installation.html)
2. Personally, I prefer \``pip install conan`\`
3. After you installed `conan`, check whether it works with \``conan --version`\` it should print \``Conan version x.x.x`\` or something like that
4. Generate default profile for conan using: \``conan profile detect --force`\` (it will automatically find your system's properties and etc)

### How to install dependencies via Conan
1. Create conan profile if you didn't: \``conan profile detect --force`\`. This will create new default profile for conan in your system (or overwrite the one that was generated before).
2. \``cd`\` to the root folder of the project
3. \``conan install . --output-folder=build --build=missing`\`
4. Congratulations! You've just installed all necessary dependencies for the project!

## Running
### Initial setup and generation of toolchain files
1. \``cd`\` to the root folder of the project
2. \``conan install . --output-folder=build --build=missing`\`
3. \``cd build`\`
4. \``cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`\`
### Building and running the executable
1. \``cd`\` to the root folder of the project
2. \``cd build`\`
3. \``cmake --build .`\`
4. \``./lab_benchmarks`\`
5. Congratulations! You've just built and run benchmarks on your own machine!

### Supported command line arguments
Thanks to the [Google Benchmark library](https://github.com/google/benchmark), there a whole bunch of useful options that one can use to specify the benchmarking process. You can see the full list on [official user guide](https://google.github.io/benchmark/user_guide.html).

I frequently used these:
1. \``./lab_benchmarks --benchmark_filter=<regex>`\` 
    
    Run benchmarks with names satisfying the given `regex`. 
    
    Example: \``--benchmark_filter=.*Building.*QubicMapBuilding`\` (will run only building phase benchmarks that use `QubicMapBuilding` algorithm)
2. \``./lab_benchmarks --benchmark_out=<filename>`\`
    
    Write benchmark results to a file. 
    
    Example: \``--benchmark_out=results.txt`\`

Wow! You're reading this! Make a tea and eat some cookies! You're truly awesome!
