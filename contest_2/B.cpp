#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>

// int GetRandomInt(int left_bound, int right_bound) {
//     std::mt19937 generator(kRandomSeed);
//     std::uniform_int_distribution<int> distrib(left_bound, right_bound);
//     int random_index = distrib(generator);
//     return random_index;
// }

// const int kRandomSeed = 668;
// std::mt19937 generator(kRandomSeed);

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// const int kMaxDepth = 30;
const int kArraySize = 10'000;

template <typename T>
void QuickSort(std::vector<T>& array, int left_index, int right_index) {
    if (left_index >= right_index) {
        return;
    }

    assert(array.size() == kArraySize);
    assert(0 <= left_index && left_index <= right_index);
    assert(left_index < array.size() && right_index < array.size());

    std::uniform_int_distribution<int> distrib(left_index, right_index);

    int pivot_index = distrib(rng);
    int pivot = array[pivot_index];

    std::swap(array[pivot_index], array[right_index]);

    int first_greater_index = left_index;
    for (int current_index = left_index; current_index < right_index;
         ++current_index) {
        if (array[current_index] <= pivot) {
            std::swap(array[first_greater_index], array[current_index]);
            ++first_greater_index;
        }
    }

    std::swap(array[first_greater_index], array[right_index]);

    bool is_splitted = true;
    for (int index = left_index; index < first_greater_index; ++index) {
        is_splitted &= (array[index] <= pivot);
    }
    for (int index = first_greater_index; index <= right_index; ++index) {
        is_splitted &= (array[index] >= pivot);
    }

    if (!is_splitted) {
        std::cout << "Mistake split --> After array \n" << std::endl;

        for (int index = left_index; index <= right_index; ++index) {
            std::cout << array[index] << ' ';
        }
        std::cout << std::endl;
        assert(false);
    }

    QuickSort(array, left_index, first_greater_index - 1);
    QuickSort(array, first_greater_index + 1, right_index);
}

void StressTest() {
    const int kEpochNum = 1000;
    const int kMinValue = -1000;
    const int kMaxValue = 1000;

    for (int epoch = 1; epoch < kEpochNum; ++epoch) {
        std::uniform_int_distribution<int> distrib(kMinValue, kMaxValue);

        std::vector<int> random_array;
        for (int index = 0; index < kArraySize; ++index) {
            random_array.push_back(distrib(rng));
        }

        // std::cout << "Base array" << std::endl;
        // for (auto x : random_array) {
        //     std::cout << x << ' ';
        // }
        // std::cout << std::endl;

        std::vector<int> custom_array = random_array;
        QuickSort(custom_array, 0, kArraySize - 1);

        std::vector<int> expected_array = random_array;
        std::sort(expected_array.begin(), expected_array.end());

        if (custom_array != expected_array) {
            std::cout << "Not equal" << std::endl;

            std::cout << "STL expected sorted array: " << std::endl;
            for (const auto& x : expected_array) {
                std::cout << x << ' ';
            }
            std::cout << std::endl;

            std::cout << "Custom sorted array: " << std::endl;
            for (const auto& x : custom_array) {
                std::cout << x << ' ';
            }
            std::cout << std::endl;

            assert(false);
        } else {
            // std::cout << " equal Epoch: " << epoch << std::endl;
        }
        // std::cout << "Current completed epoch: " << epoch << std::endl;
    }
}

int main() {
    StressTest();

    return 0;
}