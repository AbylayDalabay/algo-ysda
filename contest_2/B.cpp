#include <algorithm>
#include <cassert>
#include <cwchar>
#include <iostream>
#include <random>
#include <vector>

struct Player {
    int64_t score;
    int index;

    static bool CompByScore(const Player& lhs, const Player& rhs) {
        return lhs.score < rhs.score;
    }
    static bool CompByIndex(const Player& lhs, const Player& rhs) {
        return lhs.index < rhs.index;
    }

    bool operator==(const Player& other) const {
        return this->score == other.score && this->index == other.index;
    }
};

template <typename Iterator, typename UnaryPred>
Iterator Partition(Iterator first, Iterator last, UnaryPred pred) {
    if (std::distance(first, last) <= 1) {
        return first;
    }
    Iterator split_it = first;
    while (first != last) {
        if (pred(*first)) {
            std::iter_swap(split_it, first);
            ++split_it;
        }
        ++first;
    }
    return split_it;
}

template <typename Iterator>
Iterator SelectPivot(Iterator first, Iterator last) {
    size_t distance = std::distance(first, last);
    if (distance <= 1) {
        return first;
    }

    const int kRandomSeed = 667;
    std::mt19937 rng(kRandomSeed);
    std::uniform_int_distribution<int> distrib(0, distance - 1);

    const int kPivotIndex = distrib(rng);
    return first + kPivotIndex;
}

template <typename Iterator, typename Comparator>
void QuickSort(Iterator first, Iterator last, Comparator comparator) {
    if (std::distance(first, last) <= 1) {
        return;
    }

    const Iterator kPivotIterator = SelectPivot(first, last);

    const Iterator kEqualIterator =
        Partition(first, last, [kPivotIterator, comparator](const auto& elem) {
            return comparator(elem, *kPivotIterator);
        });

    const Iterator kGreaterIterator = Partition(
        kEqualIterator, last, [kPivotIterator, comparator](const auto& elem) {
            return !comparator(*kPivotIterator, elem);
        });

    QuickSort(first, kEqualIterator, comparator);
    QuickSort(kGreaterIterator, last, comparator);
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

    QuickSort(array.begin(), array.end(), Player::CompByScore);

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

    std::vector<Player> best_indices;
    for (int index = best_left_index; index <= best_right_index; ++index) {
        best_indices.push_back(array[index]);
    }

    QuickSort(best_indices.begin(), best_indices.end(), Player::CompByIndex);

    std::cout << best_sum << std::endl;
    for (auto player : best_indices) {
        std::cout << player.index + 1 << ' ';
    }
}