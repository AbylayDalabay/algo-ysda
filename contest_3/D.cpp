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
        std::cout << "Tree state: " << "size: " << 2 * size_ - 1 << std::endl;
        int left_index = 0;
        int right_index = 0;
        while (left_index < tree_.size() && right_index < tree_.size()) {
            for (int index = left_index; index <= right_index; ++index) {
                std::cout << tree_[index] << ' ';
            }
            std::cout << std::endl;
            left_index = LeftVertex(left_index);
            right_index = RightVertex(right_index);
        }
        std::cout << "_________________________________________" << std::endl;
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

    std::cout << "unique_array" << std::endl;
    for (auto x : unique_array) {
        std::cout << x << ' ';
    }
    std::cout << std::endl << std::endl;

    std::vector<int> permut_array(array_size);
    for (int index = 0; index < array_size; ++index) {
        permut_array[index] = Find(unique_array, array[index]);
    }

    std::cout << "permut_array" << std::endl;
    for (auto x : permut_array) {
        std::cout << x << ' ';
    }
    std::cout << std::endl << std::endl;

    SegmentTree<int> segment_tree(unique_array);

    int left_index = 0;
    int right_index = 0;

    segment_tree.Update(permut_array[0], 1);

    for (auto x : array) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    for (auto operation_type : queries_string) {
        if (operation_type == 'L') {
            segment_tree.Update(permut_array[left_index], -1);
            ++left_index;
        } else {
            ++right_index;
            segment_tree.Update(permut_array[right_index], 1);
        }

        int k_order_index = segment_tree.GetKStatistics(k_order);
        std::cout << "k_order: " << (k_order_index == -1 ? -1 : unique_array[k_order_index]) << std::endl;

        segment_tree.Print();
    }
}