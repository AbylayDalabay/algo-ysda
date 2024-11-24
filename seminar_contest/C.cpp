#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
void Display(const std::vector<std::vector<T>>& grid) {
    for (size_t row_index = 0; row_index < grid.size(); ++row_index) {
        for (size_t column_index = 0; column_index < grid[0].size();
             ++column_index) {
            std::cout << grid[row_index][column_index] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

class BiggestEqualSquares {
private:
    using HashType = uint64_t;

    static const HashType kRowPrime = 29;
    static const HashType kColumnPrime = 31;

    int row_count_;
    int column_count_;
    std::vector<std::vector<HashType>> grid_;

    std::vector<HashType> row_prime_powers_;
    std::vector<HashType> column_prime_powers_;

    std::vector<std::vector<HashType>> hash_;
    std::vector<std::vector<HashType>> prefix_sum_hash_;

    void PrecalcPowers() {
        row_prime_powers_.assign(row_count_ + 1, 1);
        for (int power = 1; power <= row_count_; ++power) {
            row_prime_powers_[power] = row_prime_powers_[power - 1] * kRowPrime;
        }

        column_prime_powers_.assign(column_count_ + 1, 1);
        for (int power = 1; power <= column_count_; ++power) {
            column_prime_powers_[power] =
                column_prime_powers_[power - 1] * kColumnPrime;
        }
    }

    void PrecalcHashes() {
        hash_.resize(row_count_, std::vector<HashType>(column_count_));
        for (int row_index = 0; row_index < row_count_; ++row_index) {
            for (int column_index = 0; column_index < column_count_;
                 ++column_index) {
                hash_[row_index][column_index] =
                    grid_[row_index][column_index] *
                    row_prime_powers_[row_index] *
                    column_prime_powers_[column_index];
            }
        }
    }

    void PrecalcPrefixSums() {
        prefix_sum_hash_.assign(row_count_ + 1,
                                std::vector<HashType>(column_count_ + 1, 0));

        for (int row_index = 1; row_index <= row_count_; ++row_index) {
            for (int column_index = 1; column_index <= column_count_;
                 ++column_index) {
                HashType current_prefix_sum = 0;
                current_prefix_sum +=
                    prefix_sum_hash_[row_index - 1][column_index];
                current_prefix_sum +=
                    prefix_sum_hash_[row_index][column_index - 1];
                current_prefix_sum -=
                    prefix_sum_hash_[row_index - 1][column_index - 1];
                current_prefix_sum += hash_[row_index - 1][column_index - 1];

                prefix_sum_hash_[row_index][column_index] = current_prefix_sum;
            }
        }
    }

    struct Cell {
        int row;
        int column;
    };

public:
    BiggestEqualSquares(const std::vector<std::string>& grid) {
        row_count_ = static_cast<int>(grid.size());
        column_count_ = static_cast<int>(grid[0].size());

        grid_.resize(row_count_, std::vector<HashType>(column_count_));
        for (int row_index = 0; row_index < row_count_; ++row_index) {
            for (int column_index = 0; column_index < column_count_;
                 ++column_index) {
                grid_[row_index][column_index] =
                    grid[row_index][column_index] - 'a';
            }
        }

        std::cout << "grid_" << std::endl;
        Display(grid_);
    }

    void Initialize() {
        PrecalcPowers();

        PrecalcHashes();
        std::cout << "hashes: " << std::endl;
        Display(hash_);

        PrecalcPrefixSums();
        std::cout << "Prefix sums" << std::endl;
        Display(prefix_sum_hash_);
    }

    HashType GetRectangleSum(Cell up, Cell down) {
        HashType answer = 0;
        answer += prefix_sum_hash_[down.row + 1][down.column + 1];
        answer -= prefix_sum_hash_[up.row][down.column + 1];
        answer -= prefix_sum_hash_[down.row + 1][up.column];
        answer += prefix_sum_hash_[up.row][up.column];

        HashType check_answer = 0;
        for (int row_index = up.row; row_index <= down.row; ++row_index) {
            for (int column_index = up.column; column_index <= down.column;
                 ++column_index) {
                check_answer += hash_[row_index][column_index];
            }
        }
        if (check_answer != answer) {
            std::cout << "check answer: " << check_answer << std::endl;
            std::cout << "answer: " << answer << std::endl;
            assert(check_answer == answer);
        }
        return answer;
    }
};

int main() {
    int row_count;
    int col_count;
    std::cin >> row_count >> col_count;

    std::vector<std::string> grid(row_count);
    for (auto& row : grid) {
        std::cin >> row;
    }

    BiggestEqualSquares solution(std::move(grid));

    solution.Initialize();

    
}