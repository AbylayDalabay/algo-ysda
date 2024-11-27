#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <vector>

template <typename T>
class Block {
private:
    struct Value {
        T value;
        int index;
    };

    int size_ = 0;
    int pref_size_ = 0;
    std::vector<Value> array_;

    static const int kBlockSize = 3;

public:
    Block() {}

    Block(Block&& other_block) {
        array_ = std::move(other_block.array_);

        size_ = other_block.size_;
        pref_size_ = other_block.pref_size_;

        other_block.size_ = 0;
        other_block.pref_size_ = 0;
    }

    Block& operator=(Block&& other_block) {
        array_ = std::move(other_block.array_);

        size_ = other_block.size_;
        pref_size_ = other_block.pref_size_;

        other_block.size_ = 0;
        other_block.pref_size_ = 0;

        return *this;
    }

    int Size() const { return size_; }

    int LeftIndex() const { return pref_size_; }
    int RightIndex() const { return pref_size_ + size_; }

    bool InsertPossible() const { return size_ < 2 * kBlockSize; }

    void PushBack(int index, T value) { array_.push_back({value, index}); }

    void Insert(int index, T value) {
        if (size_ >= 2 * kBlockSize) {
            return;
        }
        auto insert_it = array_.end();
        for (auto current_it = array_.begin(); current_it != array_.end();
             ++current_it) {
            if (current_it->value > value) {
                insert_it = current_it;
                break;
            }
        }
        array_.insert(insert_it, Value{value, index});
    }

    void Erase(int index) {
        for (auto current_it = array_.begin(); current_it != array_.end();
             ++current_it) {
            if (current_it->index == index) {
                array_.erase(current_it);
                break;
            }
        }
    }

    int BruteForceCount(int left_index, int right_index, T upper_bound) {
        int answer = 0;

        for (int global_index = LeftIndex(); global_index < RightIndex();
             ++global_index) {
            const T& current_value = array_[global_index - LeftIndex()];
            if (left_index <= global_index && global_index <= right_index &&
                current_value <= upper_bound) {
                ++answer;
            }
        }

        return answer;
    }

    Block<T> SplitAndGetSecondHalf(Block<T>& next_block) {
        assert(!InsertPossible());
        assert(size_ == 2 * kBlockSize);

        Block<T> first_block
    }

    void Display() const {
        std::cout << "Current block: ";
        for (const Value& x : array_) {
            std::cout << "{" << x.value << ", " << x.index << "} - ";
        }
        std::cout << std::endl;
    }
};

int main() {
    // int array_size;
    // std::cin >> array_size;

    // std::vector<int> array(array_size);
    // for (auto& x : array) {
    //     std::cin >> x;
    // }

    // SqrtDecomposeSet set(array);

    // set.DisplayBlocks();

    // std::cout << "Queries" << std::endl;

    // char current_operation;
    // while (std::cin >> current_operation) {
    //     if (current_operation == '-') {
    //         std::cout << "Erase operation: " << std::endl;
    //         int index;
    //         std::cin >> index;
    //         set.Erase(index);
    //     } else if (current_operation == '+') {
    //         std::cout << "Insert operation: " << std::endl;
    //         int index;
    //         int value;
    //         std::cin >> index >> value;
    //         set.Insert(index, value);
    //     } else if (current_operation == '?') {
    //         std::cout << "Count operation: " << std::endl;
    //         int left_index;
    //         int right_index;
    //         int upper_bound;

    //         std::cin >> left_index >> right_index;
    //         std::cin >> upper_bound;

    //         int answer =
    //             set.CountNotGreater(left_index, right_index, upper_bound);

    //         std::cout << "count query answer: " << answer << std::endl;
    //     }
    //     set.DisplayBlocks();
    // }

    Block<int> block;

    block.PushBack(0, 0);
    block.PushBack(0, 1);
    block.PushBack(0, 2);

    block.Display();
}