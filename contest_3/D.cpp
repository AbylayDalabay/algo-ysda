#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

template <typename T>
std::vector<T> Unique(std::vector<T> array) {
    std::vector<T> unique;
    std::sort(array.begin(), array.end());

    for (const auto& x : array) {
        if (unique.empty() || (!unique.empty() && unique.back() != x)) {
            unique.push_back(x);
        }
    }

    return unique;
}

template <typename T>
int Find(const std::vector<T>& array, const T& value) {
    int left_index = 0;
    int right_index = std::size(array) - 1;
    int find_index = -1;

    while (left_index <= right_index) {
        int mid_index = (left_index + right_index) / 2;
        if (array[mid_index] < value) {
            left_index = mid_index + 1;
        } else if (array[mid_index] > value) {
            right_index = mid_index - 1;
        } else {
            find_index = mid_index;
            break;
        }
    }

    return find_index;
}

template <typename T>
class SegmentTree {
private:
    std::vector<T> array_;
    std::vector<T> tree_;
    int array_size_;
    int size_;

    inline int LeftVertex(int vertex) const { return 2 * vertex + 1; }
    inline int RightVertex(int vertex) const { return 2 * vertex + 2; }

    void Pull(int vertex) {
        if (LeftVertex(vertex) < tree_.size() &&
            RightVertex(vertex) < tree_.size()) {
            tree_[vertex] =
                tree_[LeftVertex(vertex)] + tree_[RightVertex(vertex)];
        }
    }

    void Update(int index, T value, int vertex, int left_index,
                int right_index) {
        if (right_index - left_index == 1) {
            tree_[vertex] += value;
            return;
        }
        int mid_index = (left_index + right_index) / 2;

        if (index < mid_index) {
            Update(index, value, LeftVertex(vertex), left_index, mid_index);
        } else {
            Update(index, value, RightVertex(vertex), mid_index, right_index);
        }

        Pull(vertex);
    }

    // k_order >= 1
    int GetKStatistics(int k_order, int vertex, int left_index,
                       int right_index) {
        assert(k_order <= tree_[vertex]);

        if (right_index - left_index == 1) {
            return left_index;
        }

        int mid_index = (left_index + right_index) / 2;

        if (k_order <= tree_[LeftVertex(vertex)]) {
            return GetKStatistics(k_order, LeftVertex(vertex), left_index,
                                  mid_index);
        }
        return GetKStatistics(k_order - tree_[LeftVertex(vertex)],
                              RightVertex(vertex), mid_index, right_index);
    }

    void RecPrint(int vertex, int left_index, int right_index) {
        if (right_index - left_index == 1) {
            std::cout << tree_[vertex] << ' ';
            return;
        }
        int mid_index = (left_index + right_index) / 2;
        RecPrint(LeftVertex(vertex), left_index, mid_index);
        RecPrint(RightVertex(vertex), mid_index, right_index);
        std::cout << tree_[vertex] << ' ';
    }

public:
    SegmentTree<T>(const std::vector<T>& array)
        : array_(array), array_size_(array.size()) {
        size_ = 1;
        while (size_ < array_size_) {
            size_ *= 2;
        }

        tree_.assign(2 * size_ - 1, 0);
    }

    void Update(int index, T value) { Update(index, value, 0, 0, size_); }

    int GetKStatistics(int k_order) {
        if (k_order > tree_[0]) {
            return -1;
        }
        return GetKStatistics(k_order, 0, 0, size_);
    }

    void Print() const {
        std::cout << "Tree state: " << std::endl;
        for (int pow = 1; pow < size_; pow *= 2) {
            for (int index = pow / 2; index < pow; ++index) {
                std::cout << tree_[index] << ' ';
            }
            std::cout << std::endl;
        }
    }

    void RecPrint() { RecPrint(0, 0, size_); }
};

int main() {
    int array_size;
    int queries_count;
    int k_order;

    std::cin >> array_size >> queries_count >> k_order;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    std::string queries_string;
    std::cin >> queries_string;

    auto unique_array = Unique(array);

    for (auto& x : array) {
        x = Find(unique_array, x);
    }

    SegmentTree<int> segment_tree(unique_array);

    segment_tree.Update(0, 1);
    segment_tree.Update(1, 1);
    segment_tree.Update(2, 1);
    segment_tree.Update(2, 1);

    segment_tree.Print();

    std::cout << "Kstat: " << segment_tree.GetKStatistics(1) << std::endl;
    std::cout << "Kstat: " << segment_tree.GetKStatistics(2) << std::endl;
    std::cout << "Kstat: " << segment_tree.GetKStatistics(3) << std::endl;
    std::cout << "Kstat: " << segment_tree.GetKStatistics(4) << std::endl;

    std::cout << "_________________________" << std::endl;

    segment_tree.RecPrint();

    std::cout << std::endl;
}