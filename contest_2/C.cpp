#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

class MinHeap {
private:
    int size_;
    std::vector<int> tree_;

    static const int kInf = std::numeric_limits<int>::max();
    static int LeftChild(int index) { return 2 * index + 1; }
    static int RightChild(int index) { return 2 * index + 2; }
    static int Parent(int index) { return (index - 1) / 2; }

    int GetValue(int index) { return (index < Size() ? tree_[index] : kInf); }

    void SiftUp(int index) {
        while (index != 0) {
            if (tree_[index] < tree_[Parent(index)]) {
                std::swap(tree_[index], tree_[Parent(index)]);
                index = Parent(index);
            } else {
                break;
            }
        }
    }

    void SiftDown() {
        int current_index = 0;
        while (true) {
            int left_child_value = GetValue(LeftChild(current_index));
            int right_child_value = GetValue(RightChild(current_index));
            int min_child_index = (left_child_value <= right_child_value
                                       ? LeftChild(current_index)
                                       : RightChild(current_index));

            if (min_child_index < size_ &&
                tree_[min_child_index] < tree_[current_index]) {
                std::swap(tree_[current_index], tree_[min_child_index]);
                current_index = min_child_index;
            } else {
                break;
            }
        }
    }

    void Display() {
        std::cout << "CurrentHeap: " << std::endl;
        for (int value : tree_) {
            std::cout << value << ' ';
        }
        std::cout << std::endl;
    }

public:
    MinHeap() : size_(0) {}

    int Size() const { return size_; }

    bool Empty() const { return size_ == 0; }

    void Push(int value) {
        tree_.push_back(value);
        ++size_;
        SiftUp(size_ - 1);
    }

    int ExtractMin() {
        if (Empty()) {
            throw std::runtime_error(
                "Heap is empty. Impossible to ExtractMin().");
        }
        Display();
        int min_value = tree_[0];
        std::cout << "Min value: " << min_value << std::endl;
        tree_[0] = tree_.back();
        tree_.pop_back();
        --size_;
        SiftDown();
        Display();
        return min_value;
    }
};

int ChoosePivot(int left_index, int right_index) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(left_index, right_index);
    int pivot_index = distrib(rng);

    return pivot_index;
}

int main() {
    MinHeap min_heap;
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;

    const int kMinBound = -100;
    const int kMaxBound = 100;
    const int kEpochCount = 10000;

    for (int epoch = 0; epoch < kEpochCount; ++epoch) {
        int current_value = ChoosePivot(kMinBound, kMaxBound);
        min_heap.Push(current_value);
        pq.push(current_value);

        int pq_top = pq.top();
        pq.pop();

        int min_heap_top = min_heap.ExtractMin();

        assert(pq_top == min_heap_top);
    }

    return 0;
}