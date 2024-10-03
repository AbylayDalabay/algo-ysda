#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

struct State {
    int value, index;

    State(int value, int index) : value(value), index(index) {}

    bool operator<(const State& other) const {
        return this->value < other.value;
    }
};

class MinHeap {
private:
    int size_;
    std::vector<State> tree_;

    static const int kInf = std::numeric_limits<int>::max();
    static int LeftChild(int index) { return 2 * index + 1; }
    static int RightChild(int index) { return 2 * index + 2; }
    static int Parent(int index) { return (index - 1) / 2; }

    int GetValue(int index) {
        return (index < Size() ? tree_[index].value : kInf);
    }

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
            int min_child_index = (left_child_value < right_child_value
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

public:
    MinHeap() : size_(0) {}

    int Size() const { return size_; }

    bool Empty() const { return size_ == 0; }

    void Push(State state) {
        tree_.push_back(state);
        ++size_;
        SiftUp(size_ - 1);
    }

    State ExtractMin() {
        if (Empty()) {
            throw std::runtime_error(
                "Heap is empty. Impossible to ExtractMin().");
        }
        State min_value = tree_[0];
        tree_[0] = tree_.back();
        tree_.pop_back();
        --size_;
        SiftDown();
        return min_value;
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int row_size;
    int column_size;
    std::cin >> row_size >> column_size;
    std::vector<std::vector<int>> array(row_size,
                                        std::vector<int>(column_size, 0));
    for (int row_index = 0; row_index < row_size; ++row_index) {
        for (int column_index = 0; column_index < column_size; ++column_index) {
            std::cin >> array[row_index][column_index];
        }
    }

    MinHeap current_values;
    for (int row_index = 0; row_index < row_size; ++row_index) {
        current_values.Push(State(array[row_index][0], row_index));
    }

    std::vector<int> current_column_indices(row_size, 0);

    std::vector<int> merged_array;
    while (!current_values.Empty()) {
        State min_state = current_values.ExtractMin();
        merged_array.push_back(min_state.value);

        int current_row = min_state.index;
        ++current_column_indices[current_row];

        if (current_column_indices[current_row] < column_size) {
            current_values.Push(
                State(array[current_row][current_column_indices[current_row]],
                      current_row));
        }
    }

    for (auto value : merged_array) {
        std::cout << value << ' ';
    }
    std::cout << std::endl;

    return 0;
}