#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

template <typename T>
void QuickSort(std::vector<T>& array, int left_index, int right_index) {
    if (left_index >= right_index) {
        return;
    }

    std::uniform_int_distribution<int> distrib(left_index, right_index);

    const int kRandomSeed = 667;
    std::mt19937 rng(kRandomSeed);
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

    QuickSort(array, left_index, first_greater_index - 1);
    QuickSort(array, first_greater_index + 1, right_index);
}

int LastNotDoubleGreaterIndex(std::vector<int>& array, int left_bound,
                              int right_bound, int substract_value,
                              int not_greater_value) {
    while (left_bound + 1 < right_bound) {
        int mid_index = (left_bound + right_bound) / 2;
        if (array[mid_index] - substract_value <= not_greater_value) {
            left_bound = mid_index;
        } else {
            right_bound = mid_index;
        }
    }
    return left_bound;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int players_size;
    std::cin >> players_size;

    std::vector<int> players(players_size, 0);
    for (auto& player_score : players) {
        std::cin >> player_score;
    }

    QuickSort(players, 0, std::size(players) - 1);

    // std::sort(players.begin(), players.end());

    // std::cout << "Sorted" << std::endl;
    // for (auto x : players) {
    //     std::cout << x << ' ';
    // }
    // std::cout << std::endl;

    std::vector<int> prefix_sum(players_size + 1, 0);
    for (int index = 1; index <= players_size; ++index) {
        prefix_sum[index] = prefix_sum[index - 1] + players[index - 1];
    }

    auto get_segment_sum = [&prefix_sum](int left_bound,
                                         int right_bound) -> int64_t {
        return prefix_sum[right_bound + 1] - prefix_sum[left_bound];
    };

    int best_segment_sum = 0;
    int best_left_bound = 0;
    int best_right_bound = 0;
    for (int index = 0; index + 1 < players_size; ++index) {
        int last_double_not_greater_index =
            LastNotDoubleGreaterIndex(players, index + 1, players_size,
                                      players[index], players[index + 1]);

        int current_segment_sum =
            get_segment_sum(index, last_double_not_greater_index);

        if (current_segment_sum > best_segment_sum) {
            best_segment_sum = current_segment_sum;
            best_left_bound = index;
            best_right_bound = last_double_not_greater_index;
        }
    }

    std::cout << best_segment_sum << std::endl;
    for (int index = best_left_bound; index <= best_right_bound; ++index) {
        std::cout << index + 1 << ' ';
    }
    std::cout << std::endl;

    return 0;
}