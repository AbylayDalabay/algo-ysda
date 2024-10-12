#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <ostream>
#include <random>
#include <vector>

struct Player {
    int score, index;

    bool operator<(const Player& other) const {
        return this->score < other.score;
    }
};

int SelectPivot(int left_index, int right_index) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(left_index, right_index);
    int pivot_index = distrib(rng);

    return pivot_index;
}

template <typename T>
void Print(const std::vector<T>& array, int left_index, int right_index,
           std::string message) {
    std::cout << message << std::endl;
    std::cout << "segment: " << "[" << left_index << "; " << right_index << "]"
              << std::endl;
    for (int index = left_index; index <= right_index; ++index) {
        std::cout << array[index] << ' ';
    }
    std::cout << std::endl;
}

template <typename T>
int Partition(std::vector<T>& array, int left_index, int right_index) {
    Print(array, left_index, right_index, "full array");

    int last_index = left_index;
    int current_index = left_index;

    int pivot_index = SelectPivot(left_index, right_index);
    const T kPivotValue = array[pivot_index];

    std::swap(array[pivot_index], array[right_index]);

    while (current_index < right_index) {
        if (array[current_index] <= kPivotValue) {
            std::swap(array[last_index], array[current_index]);
            ++last_index;
            ++current_index;
        } else {
            ++current_index;
        }
    }

    std::swap(array[last_index], array[right_index]);

    Print(array, left_index, right_index, "partitioned full array");

    return last_index;
}

template <typename T>
void QuickSort(std::vector<T>& array, int left_index, int right_index) {
    if (left_index >= right_index) {
        return;
    }

    int split_index = Partition(array, left_index, right_index);

    QuickSort(array, left_index, split_index - 1);
    QuickSort(array, split_index + 1, right_index);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    QuickSort(array, 0, array_size - 1);

    std::cout << "Sorted array" << std::endl;
    for (auto x : array) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
}