#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <vector>

class FixedSet {
private:
    int size_;

    using OptionalFunction = std::optional<std::function<int(int)>>;

    OptionalFunction main_hash_func_;
    std::vector<OptionalFunction> bucket_hash_func_;

    std::vector<std::vector<std::optional<int>>> bucket_hash_table_;

    static const int64_t kInf = std::numeric_limits<int64_t>::max();
    static const int kPrimeMod = 1e9 + 7;
    static const int kCoefCollision = 5;

public:
    static int64_t CountCollisions(const std::vector<int>& array,
                                   OptionalFunction current_hash_func,
                                   int hash_table_size) {
        if (!current_hash_func.has_value()) {
            return kInf;
        }
        std::vector<int> collisions_size(hash_table_size, 0);
        for (auto x : array) {
            ++collisions_size[current_hash_func.value()(x)];
        }

        int64_t collisions_sum = 0;
        for (auto x : collisions_size) {
            collisions_sum += static_cast<int64_t>(x) * (x - 1) / 2;
        }

        return collisions_sum;
    }

    static int SelectRandom() {
        const int kRandomSeed = 667;
        static std::mt19937 random_generator(kRandomSeed);
        std::uniform_int_distribution<int> distrib(0, kPrimeMod - 1);

        return distrib(random_generator);
    }

    static OptionalFunction SelectPerfectHashFunc(const std::vector<int>& array,
                                                  int hash_table_size,
                                                  int64_t collision_limit) {
        if (hash_table_size == 0) {
            return std::nullopt;
        }

        while (true) {
            int a_param = SelectRandom();
            int b_param = SelectRandom();

            std::function<int(int)> current_hash_func =
                [a_param = a_param, b_param = b_param, prime_mod = kPrimeMod,
                 hash_table_size = hash_table_size](int value) -> int {
                auto return_value =
                    (static_cast<int64_t>(a_param) * value + b_param) %
                    prime_mod;
                if (return_value < 0) {
                    return_value += prime_mod;
                }
                return_value %= hash_table_size;
                assert(0 <= return_value && return_value < hash_table_size);
                return return_value;
            };

            auto col_sum =
                CountCollisions(array, current_hash_func, hash_table_size);

            if (col_sum <= collision_limit) {
                return OptionalFunction(current_hash_func);
            }
        }
    }

    FixedSet() {}
    void Initialize(const std::vector<int>& numbers) {
        size_ = numbers.size();
        main_hash_func_ =
            SelectPerfectHashFunc(numbers, size_, kCoefCollision * size_);

        if (!main_hash_func_.has_value()) {
            return;
        }

        std::vector<std::vector<int>> buckets(size_);
        for (auto x : numbers) {
            int hash_value = main_hash_func_.value()(x);
            buckets[hash_value].push_back(x);
        }

        bucket_hash_func_.assign(size_, std::nullopt);
        bucket_hash_table_.resize(size_);

        for (int index = 0; index < size_; ++index) {
            int current_bucket_size = buckets[index].size();
            int64_t current_bucket_hash_table_size =
                static_cast<int64_t>(current_bucket_size) * current_bucket_size;

            bucket_hash_func_[index] = SelectPerfectHashFunc(
                buckets[index], current_bucket_hash_table_size, 0);

            if (!bucket_hash_func_[index].has_value()) {
                continue;
            }

            bucket_hash_table_[index].assign(current_bucket_hash_table_size,
                                             std::nullopt);

            for (auto x : buckets[index]) {
                int hash_value = bucket_hash_func_[index].value()(x);

                bucket_hash_table_[index][hash_value] = x;
            }
        }
    }
    bool Contains(int number) const {
        if (!main_hash_func_.has_value()) {
            return false;
        }

        int bucket_index = main_hash_func_.value()(number);

        if (!bucket_hash_func_[bucket_index].has_value()) {
            return false;
        }

        int index = bucket_hash_func_[bucket_index].value()(number);

        auto found_value = bucket_hash_table_[bucket_index][index];

        if (!found_value.has_value()) {
            return false;
        }

        return found_value.value() == number;
    }
};