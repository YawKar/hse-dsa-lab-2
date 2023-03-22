#include <algorithm>
#include "PersistentSegmentTree.h"

PersistentSegmentTree::PersistentSegmentTree(std::vector<Rectangle>&& rectangles) 
: AbstractImplementation(std::move(rectangles)) {}

int PersistentSegmentTree::findPos(std::vector<int>& items, int target) {
    return std::lower_bound(items.begin(), items.end(), target) - items.begin();
}

int PersistentSegmentTree::findUpperPos(std::vector<int>& items, int target) {
    return std::upper_bound(items.begin(), items.end(), target) - items.begin();
}

int PersistentSegmentTree::queryPoint(const Point& point)
{
    if (point.x > this->zippedXs.back() || // `point` is to the right of the rightmost point of rectangles
        point.y > this->zippedYs.back() || // `point` is higher than the highest point of rectangles
        point.x < this->zippedXs.front() || // `point` is to the left of the leftmost point of rectangles
        point.y < this->zippedYs.front() // `point` is lower than the lowest point of rectangles
        ) {
        return 0; // `point` is out of the bounds
    }
    int zippedXIdx = findUpperPos(this->zippedXs, point.x) - 1;
    int zippedYIdx = findUpperPos(this->zippedYs, point.y) - 1;
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

int PersistentSegmentTree::getTotalSum(std::shared_ptr<Node> root, int left, int right, int targetIdx) {
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

std::shared_ptr<PersistentSegmentTree::Node> PersistentSegmentTree::addWithPersistence(std::shared_ptr<Node> root, int left, int right, int rangeStart, int rangeEnd, int value) {
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
    newRoot->left = addWithPersistence(newRoot->left, left, middle, rangeStart, rangeEnd, value);
    if (newRoot->right == nullptr) newRoot->right = std::shared_ptr<Node>(new Node);
    newRoot->right = addWithPersistence(newRoot->right, middle, right, rangeStart, rangeEnd, value);
    return newRoot;
}

void PersistentSegmentTree::buildInternals() {
    makeZippedCoordsFromRectangles();

    std::vector<Event> events;
    events.reserve(this->rectangles.size());
    for (const auto& rect : this->rectangles) {
        events.emplace_back(
            findPos(this->zippedXs, rect.leftDown.x),
            1,
            findPos(this->zippedYs, rect.leftDown.y),
            findPos(this->zippedYs, rect.rightUp.y + 1)
        );
        events.emplace_back(
            findPos(this->zippedXs, rect.rightUp.x + 1),
            0,
            findPos(this->zippedYs, rect.leftDown.y),
            findPos(this->zippedYs, rect.rightUp.y + 1)
        );
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
        root = addWithPersistence(root, 0, this->zippedYs.size(), events[eventIdx].zippedYIdxStart, events[eventIdx].zippedYIdxEnd, events[eventIdx].isOpening ? 1 : -1);
    }
    this->zippedRootsXIdxs.push_back(prevZippedX);
    this->roots.push_back(root);
}