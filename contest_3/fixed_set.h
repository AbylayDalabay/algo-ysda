#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <set>
#include <vector>

class FixedSet {
private:
    const int kPrimeMod = 1e9 + 7;
    const int kCoefCollisions = 5;

    int size_;

    std::optional<std::function<int(int)>> main_hash_func_;
    std::vector<std::optional<std::function<int(int)>>> bucket_hash_func_;

    std::vector<std::vector<int>> bucket_collisions_;
    std::vector<std::vector<std::optional<int>>> bucket_hash_tables_;

    static int64_t CountCollisions(const std::vector<int>& array,
                                   const std::function<int(int)>& hash_function,
                                   int hash_table_size) {
        std::vector<int> collisions_count(hash_table_size, 0);
        for (auto x : array) {
            ++collisions_count[hash_function(x)];
        }

        int64_t collisions_sum = 0;
        for (auto x : collisions_count) {
            collisions_sum += (static_cast<int64_t>(x) * (x - 1)) / 2;
        }

        return collisions_sum;
    }

    int SelectRandom() const {
        std::random_device rd;
        std::mt19937 random_generator(rd());
        std::uniform_int_distribution<int> distrib(0, kPrimeMod - 1);

        return distrib(random_generator);
    }

    std::optional<std::function<int(int)>> SelectPerfectHashFunc(
        const std::vector<int>& array, int hash_table_size,
        int64_t collision_limit) {
        if (hash_table_size == 0) {
            return std::nullopt;
        }
        while (true) {
            int a_param = SelectRandom();
            int b_param = SelectRandom();

            std::function<int(int)> current_hash_func =
                [a_param, b_param, prime_mod = kPrimeMod,
                 hash_table_size](int value) -> int {
                return ((static_cast<int64_t>(a_param) * value + b_param) %
                        prime_mod) %
                       hash_table_size;
            };

            if (CountCollisions(array, current_hash_func, hash_table_size) <=
                collision_limit) {
                return current_hash_func;
            }
        }
    }

public:
    FixedSet() {}
    void Initialize(const std::vector<int>& numbers) {
        size_ = numbers.size();
        main_hash_func_ = std::nullopt;

        if (size_ == 0) {
            return;
        }

        bucket_collisions_.resize(size_);
        bucket_hash_tables_.resize(size_);
        bucket_hash_func_.resize(size_);

        main_hash_func_ = SelectPerfectHashFunc(
            numbers, numbers.size(), kCoefCollisions * numbers.size());

        for (auto x : numbers) {
            int current_hash = main_hash_func_.value()(x);
            bucket_collisions_[current_hash].push_back(x);
        }

        for (int index = 0; index < size_; ++index) {
            if (bucket_collisions_[index].empty()) {
                continue;
            }
            int current_bucket_table_size = bucket_collisions_[index].size() *
                                            bucket_collisions_[index].size();

            bucket_hash_func_[index] = SelectPerfectHashFunc(
                bucket_collisions_[index], current_bucket_table_size, 0);

            bucket_hash_tables_[index].assign(current_bucket_table_size,
                                              std::nullopt);

            for (auto x : bucket_collisions_[index]) {
                bucket_hash_tables_[index]
                                   [bucket_hash_func_[index].value()(x)] = x;
            }
        }
    }
    bool Contains(int number) const {
        int bucket_index = main_hash_func_.value()(number);
        if (bucket_hash_tables_[bucket_index].empty()) {
            return false;
        }
        int bucket_value = bucket_hash_func_[bucket_index].value()(number);

        std::optional<int> hash_optional_value =
            bucket_hash_tables_[bucket_index][bucket_value];

        if (hash_optional_value.has_value()) {
            return hash_optional_value.value() == number;
        }

        return false;
    }
};