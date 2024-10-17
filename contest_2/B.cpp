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

template <class Iterator, class Predicate>
Iterator Partition(Iterator first, Iterator last, Predicate pred) {
    Iterator split_it = first;

    for (Iterator current_it = first; current_it != last; ++current_it) {
        if (pred(*current_it)) {
            std::swap(*split_it, *current_it);
            ++split_it;
        }
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

    const auto kPivot = *SelectPivot(first, last);

    const Iterator kEqualIterator =
        Partition(first, last, [kPivot, comparator](const auto& elem) {
            return comparator(elem, kPivot);
        });

    const Iterator kGreaterIterator =
        Partition(kEqualIterator, last, [kPivot, comparator](const auto& elem) {
            return !comparator(kPivot, elem);
        });

    QuickSort(first, kEqualIterator, comparator);
    QuickSort(kGreaterIterator, last, comparator);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    
}