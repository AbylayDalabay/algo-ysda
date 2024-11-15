#include <cstdint>
#include <iostream>
#include <vector>

int64_t CountBiggerSegments(const std::vector<int64_t>& left,
                            const std::vector<int64_t>& right,
                            int64_t threshold) {
    int64_t answer = 0;
    int left_index = 0;
    for (int right_index = 0; right_index < right.size(); ++right_index) {
        while (left_index < left.size() &&
               right[right_index] - left[left_index] < threshold) {
            ++left_index;
        }
        answer += (left.size() - left_index);
    }
    return answer;
}

std::vector<int64_t> Merge(const std::vector<int64_t>& left,
                           const std::vector<int64_t>& right) {
    std::vector<int64_t> merge;
    int left_index = 0;
    int right_index = 0;

    while (left_index < left.size() && right_index < right.size()) {
        if (left[left_index] <= right[right_index]) {
            merge.push_back(left[left_index]);
            ++left_index;
        } else {
            merge.push_back(right[right_index]);
            ++right_index;
        }
    }

    while (left_index < left.size()) {
        merge.push_back(left[left_index]);
        ++left_index;
    }

    while (right_index < right.size()) {
        merge.push_back(right[right_index]);
        ++right_index;
    }

    return merge;
}

int64_t MergeSortCount(std::vector<int64_t> array, int64_t threshold) {
    if (array.empty()) {
        return 0;
    }
    if (array.size() == 1) {
        return (array[0] >= threshold ? 1 : 0);
    }

    int64_t answer = 0;

    std::vector<int64_t> left;
    std::vector<int64_t> right;

    int pivot_index = array.size() / 2;

    for (int index = 0; index < pivot_index; ++index) {
        left.push_back(array[index]);
    }
    for (int index = pivot_index; index < array.size(); ++index) {
        right.push_back(array[index]);
    }

    left = MergeSortCount(left, threshold);
    right = MergeSortCount(right, threshold);
}

int main() {
    int array_size;
    std::cin >> array_size;
    std::vector<int64_t> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    std::vector<int64_t> prefix_sum(array_size + 1, 0);
    for (int index = 0; index < array_size; ++index) {
        prefix_sum[index + 1] = prefix_sum[index] + array[index];
    }

    int64_t min_value;
    int64_t max_value;
    std::cin >> min_value >> max_value;
}