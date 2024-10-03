#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int main() {
    int n;
    std::cin >> n;
    std::vector<int> ar(n);
    for (auto& x : ar) {
        std::cin >> x;
    }

    std::vector<std::vector<int>> suffix_max_length(n, std::vector<int>(2, 1));

    for (int i = n - 1; i >= 0; --i) {
        for (int next_i = i + 1; next_i < n; ++next_i) {
            if (ar[i] < ar[next_i]) {
                suffix_max_length[i][0] = std::max(
                    suffix_max_length[i][0], suffix_max_length[next_i][1] + 1);
            } else if (ar[i] > ar[next_i]) {
                suffix_max_length[i][1] = std::max(
                    suffix_max_length[i][1], suffix_max_length[next_i][0] + 1);
            }
        }
    }

    auto get_best_index_on_suffix = [&](int suffix_index, int flag) {
        int target_next_index = -1;

        for (int next_index = suffix_index + 1; next_index < n; ++next_index) {
            bool is_correct_dir = false;
            if (flag == 0 && ar[suffix_index] < ar[next_index]) {
                is_correct_dir = true;
            }
            if (flag == 1 && ar[suffix_index] > ar[next_index]) {
                is_correct_dir = true;
            }

            if (!is_correct_dir) {
                continue;
            }

            if (suffix_max_length[suffix_index][flag] ==
                suffix_max_length[next_index][flag ^ 1] + 1) {
                target_next_index = next_index;
                break;
            }
        }

        return target_next_index;
    };

    int best_suffix_index = 0, best_flag = 0, best_length = 1;
    for (int i = 0; i < n; ++i) {
        for (int flag = 0; flag <= 1; ++flag) {
            if (suffix_max_length[i][flag] > best_length) {
                best_length = suffix_max_length[i][flag];
                best_flag = flag;
                best_suffix_index = i;
            }
        }
    }

    std::vector<int> answer;
    while (true) {
        answer.push_back(ar[best_suffix_index]);

        int target_next_index =
            get_best_index_on_suffix(best_suffix_index, best_flag);
        if (target_next_index == -1) {
            break;
        }
        best_suffix_index = target_next_index;
        best_flag ^= 1;
    }
    assert(best_length == static_cast<int>(answer.size()));

    for (const auto& x : answer) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    return 0;
}
