#include <cassert>
#include <cstdint>
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

int SelectPivot(int left_index, int right_index) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distrib(left_index, right_index - 1);
    int pivot_index = distrib(rng);

    return pivot_index;
}

template <typename T>
void Partition(std::vector<T>& array, int left_index, int right_index,
               int& left_part_end, int& right_part_start, T pivot_value) {
    int i = left_index;
    left_part_end = left_index;
    right_part_start = right_index - 1;

    while (i <= right_part_start) {
        if (array[i] < pivot_value) {
            std::swap(array[left_part_end], array[i]);
            ++left_part_end;
            ++i;
        } else if (array[i] > pivot_value) {
            std::swap(array[i], array[right_part_start]);
            --right_part_start;
        } else {
            ++i;
        }
    }
}

template <typename T>
T GetKStatistics(std::vector<T>& array, int left_index, int right_index,
                 int current_k, int depth = 0) {
    while (true) {
        if (left_index >= right_index) {
            assert(false);
        }
        if (right_index - left_index == 1) {
            return array[left_index];
        }

        const int pivot_index = SelectPivot(left_index, right_index);
        const T pivot_value = array[pivot_index];

        int left_part_end, right_part_start;
        Partition(array, left_index, right_index, left_part_end,
                  right_part_start, pivot_value);

        if (current_k < left_part_end) {
            right_index = left_part_end;
        } else if (current_k > right_part_start) {
            left_index = right_part_start + 1;
        } else {
            return pivot_value;
        }
    }
}

int main() {
    int array_size;
    std::cin >> array_size;

    unsigned int current_a;
    unsigned int current_b;
    std::cin >> current_a >> current_b;

    Generator gen(current_a, current_b);

    std::vector<int64_t> array(array_size);
    for (int index = 0; index < array_size; ++index) {
        array[index] = gen.NextRand32();
    }

    int64_t median_value = GetKStatistics(array, 0, array_size, array_size / 2);

    int64_t answer = 0;
    for (auto x : array) {
        answer += (x >= median_value) ? x - median_value : median_value - x;
    }

    std::cout << answer << std::endl;

    return 0;
}
