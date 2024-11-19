#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>


int main() {
    int size;
    int height;
    std::cin >> size >> height;

    if (height >= size) {
        std::cout << 0 << std::endl;
        return 0;
    }

    std::vector<std::vector<double>> dp_comb(size + 5,
                                             std::vector<double>(size + 5, 0));

    dp_comb[0][0] = 1;
    for (int index = 1; index <= size; ++index) {
        dp_comb[index][0] = 1;
        for (int other_index = 1; other_index <= index; ++other_index) {
            dp_comb[index][other_index] += dp_comb[index - 1][other_index - 1];
            dp_comb[index][other_index] += dp_comb[index - 1][other_index];
        }
    }

    std::vector<std::vector<double>> dp(size + 5,
                                        std::vector<double>(height + 5, 0));

    dp[0][0] = 1;
    dp[1][0] = 1;

    for (int current_size = 2; current_size <= size; ++current_size) {
        for (int left_size = 0; left_size < current_size; ++left_size) {
            int right_size = current_size - left_size - 1;
            for (int left_height = 0; left_height <= left_size; ++left_height) {
                for (int right_height = 0; right_height <= right_size;
                     ++right_height) {
                    const double kLeftRes = dp[left_size][left_height];
                    const double kRightRes = dp[right_size][right_height];

                    const double kCurrentProd = kLeftRes * kRightRes;

                    const int kCurrentHeight =
                        std::max(left_height, right_height) + 1;

                    dp[current_size][kCurrentHeight] +=
                        kCurrentProd * dp_comb[current_size - 1][left_size];
                }
            }
        }
    }

    double summary = 0.0;
    for (int current_h = 0; current_h <= size; ++current_h) {
        summary += dp[size][current_h];
    }

    const int kPrecision = 10;

    std::cout << std::setprecision(kPrecision);
    std::cout << dp[size][height] / summary << std::endl;
}