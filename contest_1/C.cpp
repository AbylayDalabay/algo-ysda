#include <cassert>
#include <iostream>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m, len;
    std::cin >> n >> m >> len;

    std::vector<std::vector<int>> a(n, std::vector<int>(len));
    std::vector<std::vector<int>> b(m, std::vector<int>(len));

    for (int i = 0; i < n; ++i) {
        for (auto& x : a[i]) {
            std::cin >> x;
        }
    }

    for (int i = 0; i < m; ++i) {
        for (auto& x : b[i]) {
            std::cin >> x;
        }
    }

    const int MAX_VALUE = 100'000;
    std::vector<std::vector<int>> closest_pair_index(n,
                                                     std::vector<int>(m, -1));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (a[i][0] > b[j][0]) {
                closest_pair_index[i][j] = 0;
            } else {
                // exist a[i]][target_index] <= b[j][target_index]
                int left_bound = 0, right_bound = len - 1, target_index = -1;
                while (left_bound <= right_bound) {
                    int mid_index = (left_bound + right_bound) / 2;

                    if (a[i][mid_index] <= b[j][mid_index]) {
                        target_index = mid_index;
                        left_bound = mid_index + 1;
                    } else {
                        right_bound = mid_index - 1;
                    }
                }

                assert(target_index >= 0 && target_index < len);

                int left_value =
                    std::max(a[i][target_index], b[j][target_index]);
                int right_value = MAX_VALUE;
                if (target_index + 1 < len) {
                    right_value = std::max(a[i][target_index + 1],
                                           b[j][target_index + 1]);
                }

                if (left_value <= right_value) {
                    closest_pair_index[i][j] = target_index;
                } else {
                    assert(target_index + 1 < len);
                    closest_pair_index[i][j] = target_index + 1;
                }
            }
        }
    }

    int queries;
    std::cin >> queries;

    for (int query = 0; query < queries; ++query) {
        int a_index, b_index;
        std::cin >> a_index >> b_index;
        --a_index;
        --b_index;

        std::cout << closest_pair_index[a_index][b_index] + 1 << '\n';
    }

    return 0;
}