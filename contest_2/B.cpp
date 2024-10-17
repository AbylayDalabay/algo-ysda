#include <algorithm>
#include <cassert>
#include <cwchar>
#include <iostream>
#include <random>
#include <vector>

template <typename Iterator>
void Print(Iterator begin, Iterator end) {
    for (Iterator current_it = begin; current_it != end; ++current_it) {
        std::cout << *current_it << ' ';
    }
    std::cout << std::endl;
}

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

// template <typename Iterator, typename UnaryPred>
// Iterator Partition(Iterator first, Iterator last, UnaryPred pred) {
//     Iterator split_it = first;
//     for (Iterator current_it = first; current_it != last; ++current_it) {
//         if (pred(*current_it)) {
//             std::swap(*split_it, *current_it);
//             ++split_it;
//         }
//     }
//     return split_it;
// }

template <class Iterator, class Predicate>
Iterator Partition(Iterator first, Iterator last, Predicate pred) {
    Iterator split_it = first;

    std::cout << "Before partition" << std::endl;
    Print(first, last);

    for (Iterator current_it = first; current_it != last; ++current_it) {
        if (pred(*current_it)) {
            std::swap(*split_it, *current_it);
            ++split_it;
        }
    }

    std::cout << "After partition" << std::endl;
    Print(first, last);

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

    const auto kPivot = *SelectPivot(first, last);

    const Iterator kEqualIterator =
        Partition(first, last, [kPivot, comparator](const auto& elem) {
            std::cout << "elem < " << elem << " with pivot: " << kPivot
                      << " => " << (comparator(elem, kPivot) ? "YES" : "NO")
                      << std::endl;
            return comparator(elem, kPivot);
        });
    std::cout << "end pivot" << std::endl;

    std::cout << "Pivot: " << kPivot << std::endl;

    const Iterator kGreaterIterator =
        Partition(kEqualIterator, last, [kPivot, comparator](const auto& elem) {
            std::cout << "elem < " << elem << " with pivot: " << kPivot
                      << " => " << (!comparator(kPivot, elem) ? "YES" : "NO")
                      << std::endl;
            return !comparator(kPivot, elem);
        });

    std::cout << "Pivot: " << kPivot << std::endl;

    std::cout << "less:" << std::endl;
    Print(first, kEqualIterator);

    std::cout << "equal:" << std::endl;
    Print(kEqualIterator, kGreaterIterator);

    std::cout << "greater:" << std::endl;
    Print(kGreaterIterator, last);

    std::cout << "//////////////////////////////////" << std::endl;

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

int SelectPivot(int left_index, int right_index) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distrib(left_index, right_index);
    int pivot_index = distrib(rng);

    return pivot_index;
}

std::vector<int64_t> GetRandom(int array_size) {
    std::vector<int64_t> array;
    const int64_t kMaxValue = 10;
    for (int index = 0; index < array_size; ++index) {
        array.push_back(SelectPivot(0, kMaxValue));
    }
    return array;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const int kMaxEpoch = 10;
    for (int index = 0; index < kMaxEpoch; ++index) {
        auto array = GetRandom(kMaxEpoch);
        auto array_copy = array;

        std::cout << "Init array" << std::endl;
        Print(array.begin(), array.end());

        std::sort(array.begin(), array.end());
        QuickSort(array_copy.begin(), array_copy.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs < rhs; });

        if (array != array_copy) {
            Print(array.begin(), array.end());
            Print(array_copy.begin(), array_copy.end());
            std::cout << "____________________________" << std::endl;
            break;
        }
        if (array == array_copy) {
            std::cout << "Correct "
                         "00000000000000000000000000000000000000000000000000000"
                         "000000000000000000000000000000000"
                      << std::endl;
        }
    }
}