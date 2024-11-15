#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

const int64_t kMod = 123456789;

int64_t CalcBst(const std::vector<int64_t>& array, int left_index,
                int right_index,
                std::vector<std::vector<std::optional<int64_t>>>& dp) {
    if (left_index >= right_index) {
        return 1;
    }

    if (dp[left_index][right_index].has_value()) {
        return dp[left_index][right_index].value();
    }

    int64_t current_res = 0;

    for (int root_index = left_index; root_index <= right_index; ++root_index) {
        if (root_index == left_index ||
            (root_index > left_index &&
             array[root_index] > array[root_index - 1])) {
            int64_t left_res = CalcBst(array, left_index, root_index - 1, dp);
            int64_t right_res = CalcBst(array, root_index + 1, right_index, dp);

            current_res += (left_res * right_res) % kMod;
            current_res %= kMod;
        }
    }

    dp[left_index][right_index] = current_res;

    return current_res;
}

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<int64_t> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    sort(array.begin(), array.end());

    std::vector<std::vector<std::optional<int64_t>>> dp(
        array_size,
        std::vector<std::optional<int64_t>>(array_size, std::nullopt));

    std::cout << CalcBst(array, 0, array_size - 1, dp) << std::endl;
}