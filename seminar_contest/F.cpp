#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

struct SubSet {
    int64_t weight = 0;
    int64_t cost = 0;
    int64_t mask = 0;

    bool operator<(const SubSet& other) const { return cost < other.cost; }
    SubSet& operator+=(const SubSet& other) {
        weight += other.weight;
        cost += other.cost;
        mask |= other.mask;
        return *this;
    }
};

SubSet operator+(const SubSet& lhs, const SubSet& rhs) {
    return {lhs.weight + rhs.weight, lhs.cost + rhs.cost,
            (lhs.mask | rhs.mask)};
}

std::ostream& operator<<(std::ostream& os, const SubSet& subset) {
    os << "{" << subset.weight << ", " << subset.cost << ", " << subset.mask
       << "} - ";
    return os;
}

template <typename T>
void Print(const std::vector<T>& array) {
    for (const auto& x : array) {
        std::cout << x << std::endl;
    }
}

const int64_t kMax = 1e18;
const int64_t kMin = -1e18;
constexpr SubSet kMinSubset = {kMax, kMin, 0};

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
    int size_;
    int64_t min_weight_;
    int64_t max_weight_;
    std::vector<SubSet> array_;

    static std::vector<SubSet> GetMasks(std::vector<SubSet>&& array) {
        std::vector<SubSet> masks;
        int size = array.size();
        for (int mask = 0; mask < (1 << size); ++mask) {
            SubSet current_subset = {0, 0, 0};
            for (int current_bit = 0; current_bit < size; ++current_bit) {
                if ((mask & (1 << current_bit)) > 0) {
                    current_subset += array[current_bit];
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
    SubSet StressSolve() {
        SubSet answer = kMinSubset;
        for (int mask = 0; mask < (1 << size_); ++mask) {
            SubSet current_subset = {0, 0, 0};
            for (int current_bit = 0; current_bit < size_; ++current_bit) {
                if ((mask & (1 << current_bit)) > 0) {
                    current_subset += array_[current_bit];
                }
            }
            if (current_subset.weight >= min_weight_ &&
                current_subset.weight <= max_weight_) {
                answer = std::max(answer, current_subset);
            }
        }
        return answer;
    }
    SubSet Solve() {
        int mid_index = size_ / 2;
        auto mid_iterator = array_.begin() + mid_index;

        std::vector<SubSet> left_masks =
            GetMasks(std::vector(array_.begin(), mid_iterator));
        std::vector<SubSet> right_masks =
            GetMasks(std::vector(mid_iterator, array_.end()));

        auto compare_by_weight = [](const SubSet& lhs, const SubSet& rhs) {
            return lhs.weight < rhs.weight;
        };

        sort(left_masks.begin(), left_masks.end(), compare_by_weight);
        sort(right_masks.begin(), right_masks.end(), compare_by_weight);

        int right_masks_size = right_masks.size();
        SegmentTree<SubSet> seg_tree(right_masks_size, kMinSubset);
        for (int index = 0; index < right_masks_size; ++index) {
            seg_tree.Update(index, right_masks[index]);
        }

        SubSet answer = kMinSubset;
        for (const auto& left_mask : left_masks) {
            int64_t left_bound = min_weight_ - left_mask.weight;
            int64_t right_bound = max_weight_ - left_mask.weight;

            int left_index = FindFirstNotLessByWeight(right_masks, left_bound);
            int right_index =
                FindLastNotGreaterByWeight(right_masks, right_bound);

            if (left_index != -1 && right_index != -1 &&
                left_index <= right_index) {
                for (int index = left_index; index <= right_index; ++index) {
                    assert(right_masks[index].weight >= left_bound);
                    assert(right_masks[index].weight <= right_bound);
                }

                SubSet max_right_subset =
                    seg_tree.GetMax(left_index, right_index);
                SubSet current_res = left_mask + max_right_subset;

                assert(current_res.weight >= min_weight_);
                assert(current_res.weight <= max_weight_);

                answer = std::max(answer, current_res);
            }
        }

        std::cout << __builtin_popcount(answer.mask) << std::endl;
        for (int current_bit = 0; current_bit < size_; ++current_bit) {
            if ((answer.mask & (1 << current_bit)) > 0) {
                std::cout << current_bit + 1 << ' ';
            }
        }
        std::cout << std::endl;

        return answer;
    }
};

int main() {
    Solution kotak;
    kotak.Input();
    auto solve = kotak.Solve();
}