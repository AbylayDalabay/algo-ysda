#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <ostream>
#include <random>
#include <vector>

struct SubSet {
    int64_t weight = 0;
    int64_t cost = 0;
    int64_t mask = 0;

    bool operator<(const SubSet& other) const { return cost < other.cost; }
    bool operator==(const SubSet& other) const {
        return (weight == other.weight) && (cost == other.cost) &&
               (mask == other.mask);
    }
    bool operator!=(const SubSet& other) const { return !(*this == other); }
};

const int64_t kMax = std::numeric_limits<int64_t>::max();
const int64_t kMin = std::numeric_limits<int64_t>::min();
constexpr SubSet kMinSubset = {kMax, kMin, 0};

std::ostream& operator<<(std::ostream& os, const SubSet& subset) {
    os << "{" << std::endl;
    os << "  weight: " << subset.weight << std::endl;
    os << "  cost: " << subset.cost << std::endl;
    os << "  mask: " << subset.mask << std::endl;
    os << "}" << std::endl;
    return os;
}

template <typename T>
void Print(const std::vector<T>& array) {
    for (const auto& x : array) {
        std::cout << x << std::endl;
    }
}

template <typename T>
class SegmentTree {
private:
    int size_;
    std::vector<T> tree_;
    const T kDefaultValue;

    void Pull(int vertex, int left_index, int right_index) {
        if (right_index - left_index == 1) {
            return;
        }
        tree_[vertex] = std::max(tree_[2 * vertex + 1], tree_[2 * vertex + 2]);
    }
    void Update(int index, const T& value, int vertex, int left_index,
                int right_index) {
        if (right_index - left_index == 1) {
            tree_[vertex] = value;
            return;
        }
        int mid_index = (left_index + right_index) / 2;
        if (index < mid_index) {
            Update(index, value, 2 * vertex + 1, left_index, mid_index);
        } else {
            Update(index, value, 2 * vertex + 2, mid_index, right_index);
        }
        Pull(vertex, left_index, right_index);
    }
    T GetMax(int left_query, int right_query, int vertex, int left_index,
             int right_index) {
        if (right_query <= left_index || right_index <= left_query) {
            return kDefaultValue;
        }
        if (left_query <= left_index && right_index <= right_query) {
            return tree_[vertex];
        }
        int mid_index = (left_index + right_index) / 2;
        auto left_res = GetMax(left_query, right_query, 2 * vertex + 1,
                               left_index, mid_index);
        auto right_res = GetMax(left_query, right_query, 2 * vertex + 2,
                                mid_index, right_index);

        return std::max(left_res, right_res);
    }

public:
    SegmentTree(int tree_size, const T& default_value)
        : kDefaultValue(default_value) {
        size_ = 1;
        while (size_ < tree_size) {
            size_ *= 2;
        }
        tree_.assign(2 * size_ - 1, default_value);
    }
    void Update(int index, const T& value) {
        Update(index, value, 0, 0, size_);
    }
    T GetMax(int left_query, int rigth_query) {
        return GetMax(left_query, rigth_query + 1, 0, 0, size_);
    }
};

class Solution {
private:
    static const int64_t kMin = std::numeric_limits<int64_t>::min();
    static const int64_t kMax = std::numeric_limits<int64_t>::max();
    constexpr static SubSet kMinSubset = {kMin, kMax, 0};

    int size_;
    int64_t min_weight_;
    int64_t max_weight_;
    std::vector<SubSet> array_;

    static std::vector<SubSet> GetMasks(std::vector<SubSet>&& array) {
        std::vector<SubSet> masks;
        int size = array.size();
        for (int mask = 1; mask < (1 << size); ++mask) {
            SubSet current_subset = {0, 0, mask};
            for (int current_bit = 0; current_bit < size; ++current_bit) {
                if ((mask & (1 << current_bit)) > 0) {
                    current_subset.weight += array[current_bit].weight;
                    current_subset.cost += array[current_bit].cost;
                }
            }
            masks.push_back(current_subset);
        }
        return masks;
    }
    static int FindFirstNotLessByWeight(const std::vector<SubSet>& array,
                                        int64_t target_weight) {
        int left_index = 0;
        int right_index = static_cast<int>(array.size()) - 1;
        int best_index = -1;
        while (left_index <= right_index) {
            int mid_index = (left_index + right_index) / 2;
            if (array[mid_index].weight >= target_weight) {
                best_index = mid_index;
                right_index = mid_index - 1;
            } else {
                left_index = mid_index + 1;
            }
        }
        return best_index;
    }
    static int FindLastNotGreaterByWeight(const std::vector<SubSet>& array,
                                          int64_t target_weight) {
        int left_index = 0;
        int right_index = static_cast<int>(array.size()) - 1;
        int best_index = -1;
        while (left_index <= right_index) {
            int mid_index = (left_index + right_index) / 2;
            if (array[mid_index].weight <= target_weight) {
                best_index = mid_index;
                left_index = mid_index + 1;
            } else {
                right_index = mid_index - 1;
            }
        }
        return best_index;
    }

public:
    void Input() {
        std::cin >> size_;
        std::cin >> min_weight_;
        std::cin >> max_weight_;

        array_.resize(size_);
        for (int index = 0; index < size_; ++index) {
            std::cin >> array_[index].weight;
            std::cin >> array_[index].cost;
            array_[index].mask = (1 << index);
        }
    }
    void Solve() {
        int mid_index = size_ / 2;
        auto mid_iterator = array_.begin() + mid_index;

        std::vector<SubSet> left_masks =
            GetMasks(std::vector(array_.begin(), mid_iterator));
        std::vector<SubSet> right_masks =
            GetMasks(std::vector(mid_iterator, array_.end()));

        sort(left_masks.begin(), left_masks.end());
        sort(right_masks.begin(), right_masks.end());

        SubSet answer = kMinSubset;
        for (const auto& left_mask : left_masks) {
            int left_bound = min_weight_ - left_mask.weight;
            int right_bound = max_weight_ - left_mask.weight;

            int left_index = FindFirstNotLessByWeight(right_masks, left_bound);
            int right_index =
                FindLastNotGreaterByWeight(right_masks, right_bound);

            if (left_index != -1 && right_index != -1 &&
                left_index <= right_index) {
                SubSet max_right_subset;
            }
        }

        // std::cout << "left masks: " << std::endl;
        // Print(left_masks);

        // std::cout << "right masks: " << std::endl;
        // Print(right_masks);
    }
};

std::random_device rd;
std::mt19937_64 rng(rd());

int64_t SelectRandom(int64_t left_bound, int64_t right_bound) {
    std::uniform_int_distribution<int> distrib(left_bound, right_bound);
    return distrib(rng);
}

int main() {
    // Solution kotak;
    // kotak.Input();
    // kotak.Solve();

    int size;
    std::cin >> size;

    std::vector<SubSet> array(size);
    for (int index = 0; index < size; ++index) {
        array[index].weight = SelectRandom(0, kMax);
        array[index].cost = SelectRandom(0, kMax);
        array[index].mask = 0;
    }

    SegmentTree<SubSet> segtree(size, kMinSubset);
    for (int index = 0; index < size; ++index) {
        segtree.Update(index, array[index]);
    }

    bool ok = true;
    for (int left_index = 0; left_index < size; ++left_index) {
        for (int right_index = left_index; right_index < size; ++right_index) {
            SubSet segtree_max = segtree.GetMax(left_index, right_index);
            SubSet brute_max = kMinSubset;
            for (int index = left_index; index <= right_index; ++index) {
                brute_max = std::max(brute_max, array[index]);
            }

            ok &= (brute_max == segtree_max);
            if (brute_max != segtree_max) {
                std::cout << "brute_max: " << brute_max;
                std::cout << "segtree_max: " << segtree_max;
                assert(brute_max != segtree_max);
            }
        }
    }

    std::cout << "Stress Test: " << std::boolalpha << ok << std::endl;
}