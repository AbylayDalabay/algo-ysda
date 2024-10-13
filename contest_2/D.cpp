#include <cassert>
#include <iostream>
#include <random>
#include <regex>
#include <vector>

template <typename T>
void Print(const std::vector<T>& array, int left_index, int right_index) {
    for (int index = left_index; index < right_index; ++index) {
        std::cout << array[index] << ' ';
    }
    std::cout << std::endl;
}

class Generator {
private:
    unsigned int a_, b_;
    unsigned int cur_ = 0;

    static const unsigned int kEight = 8;

    unsigned int NextRand24() {
        cur_ = cur_ * a_ + b_;  // вычисляется с переполнениями
        return cur_ >> kEight;  // число от 0 до 2**24-1.
    }

public:
    Generator(unsigned int current_a, unsigned int current_b)
        : a_(current_a), b_(current_b) {}

    unsigned int NextRand32() {
        unsigned int current_a = NextRand24();
        unsigned int current_b = NextRand24();
        return (current_a << kEight) ^ current_b;  // число от 0 до 2**32-1.
    }
};

const int kRandomSeed = 667;
std::mt19937 rng(kRandomSeed);

int SelectPivot(int left_index, int right_index) {
    std::uniform_int_distribution<int> distrib(left_index, right_index - 1);
    int pivot_index = distrib(rng);

    return pivot_index;
}

template <class T, class UnaryPred>
int Partition(std::vector<T>& array, int left_index, int right_index,
              UnaryPred predicat) {
    int split_index = left_index;
    for (int index = left_index; index < right_index; ++index) {
        if (predicat(array[index])) {
            std::swap(array[split_index], array[index]);
            ++split_index;
        }
    }

    return split_index;
}

template <typename T>
T GetKStatistics(std::vector<T>& array, int left_index, int right_index,
                 int current_k) {
    std::cout << "[" << left_index << ", " << right_index << "]" << std::endl;
    std::cout << "Current K: " << current_k << std::endl;
    if (left_index >= right_index) {
        assert(false);
    } else if (right_index - left_index == 1) {
        assert(current_k == 0);
        return array[left_index];
    }

    const int kPivotIndex = SelectPivot(left_index, right_index);
    const int kPivot = array[kPivotIndex];

    const int kEqualIndex =
        Partition(array, left_index, right_index,
                  [kPivot](const auto& elem) { return (elem < kPivot); });

    const int kGreaterIndex =
        Partition(array, kEqualIndex, right_index,
                  [kPivot](const auto& elem) { return (elem == kPivot); });

    std::cout << "[" << left_index << ", " << kEqualIndex << ", "
              << kGreaterIndex << ", " << right_index << "]" << std::endl;

    std::cout << "__________________________" << std::endl;
    std::cout << "Pivot: " << kPivot << std::endl;
    std::cout << "Current K: " << current_k << std::endl;

    std::cout << "less: " << std::endl;
    Print(array, left_index, kEqualIndex);
    std::cout << "equal: " << std::endl;
    Print(array, kEqualIndex, kGreaterIndex);
    std::cout << "greater: " << std::endl;
    Print(array, kGreaterIndex, right_index);

    std::cout << "__________________________" << std::endl;

    if (left_index + current_k < kEqualIndex) {
        std::cout << "go to less" << std::endl;
        return GetKStatistics(array, left_index, kEqualIndex, current_k);
    }
    if (kEqualIndex <= left_index + current_k &&
        left_index + current_k < kGreaterIndex) {
        std::cout << "go to equal" << std::endl;
        return array[left_index + current_k];
    }
    std::cout << "go to greater" << std::endl;
    return GetKStatistics(array, kGreaterIndex, right_index,
                          left_index + current_k - kGreaterIndex);
}

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    std::vector<int> array_copy = array;
    std::sort(array_copy.begin(), array_copy.end());

    int sex = 0;
    for (int current_k = 0; current_k < array_size; ++current_k) {
        int target_value = GetKStatistics(array, 0, array_size, current_k);

        if (target_value != array_copy[current_k]) {
            std::cout << target_value << ", " << array_copy[current_k]
                      << std::endl;
            assert(false);
        } else {
            ++sex;
        }
    }

    std::cout << "Correct: " << sex << std::endl;

    // const int kPivot = SelectPivot(0, array_size);

    // int split_index =
    //     Partition(array, 0, array_size,
    //               [kPivot](const auto& elem) { return (elem < kPivot); });

    // Print(array, 0, split_index);
    // Print(array, split_index, array_size);

    // int current_a;
    // int current_b;
    // std::cin >> current_a >> current_b;

    // Generator gen(current_a, current_b);

    // std::vector<unsigned int> array;
    // for (int index = 0; index < array_size; ++index) {
    //     array.push_back(gen.NextRand32());
    // }

    // for (auto x : array) {
    //     std::cout << x << ' ';
    // }
    // std::cout << std::endl;

    return 0;
}