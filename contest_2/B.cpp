#include <algorithm>
#include <cassert>
#include <cwchar>
#include <iostream>
#include <vector>

struct Player {
    int64_t score;
    int index;

    bool operator<(const Player& other) const {
        return this->score < other.score;
    }
};

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

    std::sort(array.begin(), array.end());

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

    std::sort(best_indices.begin(), best_indices.end());

    std::cout << best_sum << std::endl;
    for (int index : best_indices) {
        std::cout << index + 1 << ' ';
    }
}