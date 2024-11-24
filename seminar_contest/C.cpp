#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class BiggestEqualSquares {
private:
    using HashType = int64_t;

    static const HashType kRowPrime = 31;
    static const HashType kColumnPrime = 31;

    static const HashType kMod = 1e9 + 7;

    struct Cell {
        int row;
        int column;
    };

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
            row_prime_powers_[power] =
                (row_prime_powers_[power - 1] * kRowPrime) % kMod;
        }

        column_prime_powers_.assign(column_count_ + 1, 1);
        for (int power = 1; power <= column_count_; ++power) {
            column_prime_powers_[power] =
                (column_prime_powers_[power - 1] * kColumnPrime) % kMod;
        }
    }

    void PrecalcHashes() {
        hash_.resize(row_count_, std::vector<HashType>(column_count_));
        for (int row_index = 0; row_index < row_count_; ++row_index) {
            for (int column_index = 0; column_index < column_count_;
                 ++column_index) {
                hash_[row_index][column_index] =
                    (((grid_[row_index][column_index] *
                       row_prime_powers_[row_index]) %
                      kMod) *
                     column_prime_powers_[column_index]) %
                    kMod;
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

                prefix_sum_hash_[row_index][column_index] =
                    (current_prefix_sum % kMod + kMod) % kMod;
            }
        }
    }

    HashType GetRectangleSumHash(Cell up, Cell down) {
        HashType answer = 0;
        answer += prefix_sum_hash_[down.row + 1][down.column + 1];
        answer -= prefix_sum_hash_[up.row][down.column + 1];
        answer -= prefix_sum_hash_[down.row + 1][up.column];
        answer += prefix_sum_hash_[up.row][up.column];

        return (answer % kMod + kMod) % kMod;
    }

    bool IsEqualRectangles(const Cell& first, const Cell& second,
                           int side_length) {
        bool is_equal = true;
        for (int add_row_index = 0; add_row_index < side_length;
             ++add_row_index) {
            for (int add_column_index = 0; add_column_index < side_length;
                 ++add_column_index) {
                is_equal &= (grid_[first.row + add_row_index]
                                  [first.column + add_column_index] ==
                             grid_[second.row + add_row_index]
                                  [second.column + add_column_index]);

                if (!is_equal) {
                    return false;
                }
            }
        }
        return true;
    }

    std::optional<std::pair<Cell, Cell>> CheckGoodCollisions(
        const std::vector<Cell>& cells, int side_length) {
        for (size_t first_index = 0; first_index < cells.size();
             ++first_index) {
            for (size_t second_index = first_index + 1;
                 second_index < cells.size(); ++second_index) {
                if (IsEqualRectangles(cells[first_index], cells[second_index],
                                      side_length)) {
                    return std::make_pair(cells[first_index],
                                          cells[second_index]);
                }
            }
        }
        return std::nullopt;
    }

    HashType NormalizeCellHashMultilplier(const Cell& cell) {
        HashType row_mult = row_prime_powers_[row_count_ - cell.row];
        HashType column_mult =
            column_prime_powers_[column_count_ - cell.column];
        return (row_mult * column_mult) % kMod;
    }

    std::optional<std::pair<Cell, Cell>> FindEqualSquares(int side_length) {
        std::unordered_map<HashType, std::vector<Cell>> collisions;

        for (int row_index = 0; row_index + side_length - 1 < row_count_;
             ++row_index) {
            for (int column_index = 0;
                 column_index + side_length - 1 < column_count_;
                 ++column_index) {
                Cell start_cell = {row_index, column_index};
                Cell end_cell = {row_index + side_length - 1,
                                 column_index + side_length - 1};

                HashType current_rectangle_hash =
                    GetRectangleSumHash(start_cell, end_cell);

                current_rectangle_hash *=
                    NormalizeCellHashMultilplier(start_cell);

                current_rectangle_hash %= kMod;

                collisions[current_rectangle_hash].push_back(start_cell);
            }
        }

        for (const auto& current_hash_rectangles : collisions) {
            if (current_hash_rectangles.second.size() > 1) {
                std::optional<std::pair<Cell, Cell>> equal_squares =
                    CheckGoodCollisions(current_hash_rectangles.second,
                                        side_length);

                if (equal_squares.has_value()) {
                    return equal_squares;
                }
            }
        }

        return std::nullopt;
    }

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
    }

    void InitializePrecalc() {
        PrecalcPowers();

        PrecalcHashes();

        PrecalcPrefixSums();
    }

    void CalcAnswer() {
        int left_bound = 1;
        int right_bound = std::min(row_count_, column_count_);
        int best_bound = 0;
        std::optional<std::pair<Cell, Cell>> equal_squares;

        while (left_bound <= right_bound) {
            int mid_bound = (left_bound + right_bound) / 2;
            std::optional<std::pair<Cell, Cell>> current_equal_squares =
                FindEqualSquares(mid_bound);

            if (current_equal_squares.has_value()) {
                left_bound = mid_bound + 1;
                best_bound = mid_bound;
                equal_squares = current_equal_squares;
            } else {
                right_bound = mid_bound - 1;
            }
        }

        if (equal_squares.has_value()) {
            std::cout << best_bound << std::endl;
            Cell first_cell = equal_squares.value().first;
            Cell second_cell = equal_squares.value().second;

            std::cout << first_cell.row + 1 << ' ' << first_cell.column + 1
                      << std::endl;
            std::cout << second_cell.row + 1 << ' ' << second_cell.column + 1
                      << std::endl;
        } else {
            std::cout << 0 << std::endl;
        }
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

    solution.InitializePrecalc();

    solution.CalcAnswer();
}