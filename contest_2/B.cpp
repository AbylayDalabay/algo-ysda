#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <ostream>
#include <random>
#include <vector>

struct Player {
    int64_t score, index;

    bool operator<(const Player& other) const {
        return this->score < other.score;
    }

    bool operator==(const Player& other) const {
        return this->score == other.score;
    }

    bool operator<=(const Player& other) const {
        return (*this < other) || (*this == other);
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
int Partition(std::vector<T>& array, int left_index, int right_index) {
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

    return last_index;
}

template <typename T>
void QuickSort(std::vector<T>& array, int left_index, int right_index,
               int depth = 0) {
    if (left_index >= right_index) {
        return;
    }

    int split_index = Partition(array, left_index, right_index);

    QuickSort(array, left_index, split_index - 1, depth + 1);
    QuickSort(array, split_index + 1, right_index, depth + 1);
}

int GetLastNotGreater(const std::vector<Player>& array, int left_index,
                      int right_index, int64_t not_greater_value) {
    int target_index = left_index;
    while (left_index <= right_index) {
        int mid_index = (left_index + right_index) / 2;
        if (array[mid_index].score <= not_greater_value) {
            target_index = mid_index;
            left_index = mid_index + 1;
        } else {
            right_index = mid_index - 1;
        }
    }
    return target_index;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int array_size;
    std::cin >> array_size;

    std::vector<Player> array(array_size);
    for (int index = 0; index < array_size; ++index) {
        std::cin >> array[index].score;
        array[index].index = index;
    }

    QuickSort(array, 0, array_size - 1);

    std::vector<int64_t> pref(array_size + 1, 0);
    for (int index = 1; index <= array_size; ++index) {
        pref[index] = pref[index - 1] + array[index - 1].score;
    }
    auto get_sum = [&](int left_index, int right_index) -> int64_t {
        return pref[right_index + 1] - pref[left_index];
    };
    int best_left_index = 0;
    int best_right_index = 0;
    int64_t best_sum = 0;
    for (int index = 0; index < array_size; ++index) {
        if (array[index].score > best_sum) {
            best_left_index = index;
            best_right_index = index;
            best_sum = array[index].score;
        }
    }
    for (int index = 0; index + 1 < array_size; ++index) {
        int target_index =
            GetLastNotGreater(array, index + 1, array_size - 1,
                              array[index].score + array[index + 1].score);

        if (get_sum(index, target_index) > best_sum) {
            best_sum = get_sum(index, target_index);
            best_left_index = index;
            best_right_index = target_index;
        }
    }

    std::vector<int> best_indices;
    for (int index = best_left_index; index <= best_right_index; ++index) {
        best_indices.push_back(array[index].index);
    }

    QuickSort(best_indices, 0, best_indices.size() - 1);

    std::cout << best_sum << std::endl;
    for (int index : best_indices) {
        std::cout << index + 1 << ' ';
    }
    std::cout << std::endl;
}