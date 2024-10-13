#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cwchar>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

struct Player {
    int skill;
    int index;

    static bool CompareBySkill(const Player& lhs, const Player& rhs) {
        return lhs.skill < rhs.skill;
    }

    static bool CompareByIndex(const Player& lhs, const Player& rhs) {
        return lhs.index < rhs.index;
    }
};

template <class Iterator>
void Print(Iterator left, Iterator right) {
    for (Iterator current_it = left; current_it != right; ++current_it) {
        std::cout << *current_it << ' ';
    }
    std::cout << std::endl;
}

const int kRandomSeed = 667;
std::mt19937 rng(kRandomSeed);

template <typename Iterator>
Iterator SelectPivot(Iterator left, Iterator right) {
    const int kCurrentSize = std::distance(left, right);

    std::uniform_int_distribution<int> distrib(0, kCurrentSize - 1);

    int pivot_index = distrib(rng);

    return left + pivot_index;
}

template <class Iterator, class UnaryPred>
Iterator Partition(Iterator left, Iterator right, UnaryPred predicat) {
    std::cout << "before partition: " << std::endl;
    Print(left, right);
    Iterator split_it = left;
    for (Iterator current_it = left; current_it != right; ++current_it) {
        if (predicat(current_it)) {
            std::iter_swap(current_it, split_it);
            ++split_it;
        }
    }
    std::cout << "after partition: " << std::endl;
    Print(left, right);
    return split_it;
}

template <class Iterator, typename Comparator>
void QuickSort(Iterator left, Iterator right, const Comparator& comparator) {
    if (left >= right) {
        return;
    }

    const Iterator kPivot = SelectPivot(left, right);

    const Iterator kEqual =
        Partition(left, right, [kPivot, &comparator](const Iterator& it) {
            return comparator(*it, *kPivot);
        });
    const Iterator kGreater =
        Partition(kEqual, right, [kPivot, &comparator](const Iterator& it) {
            return !comparator(*kPivot, *it);
        });

    std::cout << "Pivot: " << *kPivot << std::endl;
    std::cout << "less: " << std::endl;
    Print(left, kEqual);
    std::cout << "equal: " << std::endl;
    Print(kEqual, kGreater);
    std::cout << "greater: " << std::endl;
    Print(kGreater, right);
    std::cout << "________________________" << std::endl;

    QuickSort(left, kEqual, comparator);
    QuickSort(kGreater, right, comparator);
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

    QuickSort(array.begin(), array.end(), std::greater<int>());

    Print(array.begin(), array.end());
}