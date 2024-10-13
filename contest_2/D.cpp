#include <sys/types.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <random>
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

    if (left_index + current_k < kEqualIndex) {
        return GetKStatistics(array, left_index, kEqualIndex, current_k);
    }
    if (kEqualIndex <= left_index + current_k &&
        left_index + current_k < kGreaterIndex) {
        return array[left_index + current_k];
    }
    return GetKStatistics(array, kGreaterIndex, right_index,
                          left_index + current_k - kGreaterIndex);
}

int main() {
    int array_size;
    std::cin >> array_size;

    unsigned int current_a;
    unsigned int current_b;
    std::cin >> current_a >> current_b;

    Generator gen(current_a, current_b);

    std::vector<int64_t> array;
    for (int index = 0; index < array_size; ++index) {
        array.push_back(gen.NextRand32());
        std::cout << array.back() << ' ';
    }
    std::cout << std::endl;

    int64_t median_value = GetKStatistics(array, 0, array_size, array_size / 2);

    std::cout << median_value << std::endl;

    int64_t answer = 0;
    for (auto x : array) {
        answer += std::abs(x - median_value);
    }

    std::cout << answer << std::endl;

    for (auto x : array) {
        int64_t current_answer = 0;
        for (auto y : array) {
            current_answer += std::abs(x - y);
        }
        std::cout << x << " => " << current_answer << std::endl;
    }

    std::sort(array.begin(), array.end());
    std::cout << "array: " << std::endl;
    for (auto x : array) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    return 0;
}