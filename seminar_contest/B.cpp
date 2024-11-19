#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

template <typename T>
std::vector<T> Merge(const std::vector<T>& left, const std::vector<T>& right) {
    std::vector<T> merge;

    size_t ptr_left = 0;
    size_t ptr_right = 0;

    while (ptr_left < left.size() && ptr_right < right.size()) {
        if (left[ptr_left] <= right[ptr_right]) {
            merge.push_back(left[ptr_left]);
            ++ptr_left;
        } else {
            merge.push_back(right[ptr_right]);
            ++ptr_right;
        }
    }

    while (ptr_left < left.size()) {
        merge.push_back(left[ptr_left]);
        ++ptr_left;
    }

    while (ptr_right < right.size()) {
        merge.push_back(right[ptr_right]);
        ++ptr_right;
    }

    return merge;
}

template <typename T>
std::vector<T> MergeSort(std::vector<T> array, T threshold, int64_t& answer) {
    if (array.size() <= 1) {
        return array;
    }

    size_t pivot_index = array.size() / 2;

    std::vector<T> left;
    std::vector<T> right;

    for (size_t index = 0; index < pivot_index; ++index) {
        left.push_back(array[index]);
    }
    for (size_t index = pivot_index; index < array.size(); ++index) {
        right.push_back(array[index]);
    }

    left = MergeSort(left, threshold, answer);
    right = MergeSort(right, threshold, answer);

    size_t ptr_right = 0;
    for (auto current_left : left) {
        while (ptr_right < right.size() &&
               right[ptr_right] < current_left + threshold) {
            ++ptr_right;
        }

        answer += (right.size() - ptr_right);
    }

    return Merge(left, right);
}

int64_t CountNotLessSegments(const std::vector<int64_t>& array,
                             int64_t threshold) {
    int64_t answer = 0;
    MergeSort(array, threshold, answer);
    return answer;
}

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<int64_t> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    std::vector<int64_t> prefix_sum(array_size + 1, 0);
    for (int index = 1; index <= array_size; ++index) {
        prefix_sum[index] = prefix_sum[index - 1] + array[index - 1];
    }

    int64_t min_bound;
    int64_t max_bound;
    std::cin >> min_bound >> max_bound;

    int64_t bigger_max_bound_count =
        CountNotLessSegments(prefix_sum, max_bound + 1);
    int64_t bigger_min_bound_count =
        CountNotLessSegments(prefix_sum, min_bound);

    std::cout << bigger_min_bound_count - bigger_max_bound_count << std::endl;
}