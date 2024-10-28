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
                                   const OptionalFunction& current_hash_func,
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
        std::random_device rd;
        std::mt19937 random_generator(rd());
        std::uniform_int_distribution<int> distrib(0, kPrimeMod - 1);

        return distrib(random_generator);
    }

    static OptionalFunction SelectPerfectHashFunc(const std::vector<int>& array,
                                                  int hash_table_size,
                                                  int64_t collision_limit) {
        if (hash_table_size == 0) {
            return OptionalFunction(std::nullopt);
        }

        int epoch_count = 0;
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

            ++epoch_count;
            if (CountCollisions(array, current_hash_func, hash_table_size) <=
                collision_limit) {
                std::cout << "Converged after: " << epoch_count << std::endl;
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

        std::cout << "main_hash completed" << std::endl;

        std::vector<std::vector<int>> buckets(size_);
        for (auto x : numbers) {
            int hash_value = main_hash_func_.value()(x);
            buckets[hash_value].push_back(x);
        }

        std::cout << "buckets: " << std::endl;
        for (auto bucket : buckets) {
            std::cout << "bucket: ";
            for (auto x : bucket) {
                std::cout << x << ' ';
            }
            std::cout << std::endl;
        }

        bucket_hash_func_.assign(size_, std::nullopt);
        bucket_hash_table_.resize(size_);

        for (int index = 0; index < size_; ++index) {
            int current_bucket_size = buckets[index].size();
            int64_t current_bucket_hash_table_size =
                static_cast<int64_t>(current_bucket_size) * current_bucket_size;

            std::cout << index << " started" << std::endl;

            auto hui = SelectPerfectHashFunc(buckets[index],
                                             current_bucket_hash_table_size, 0);

            std::cout << index << " returned" << std::endl;

            bucket_hash_func_[index] = hui;

            std::cout << index << " ended" << std::endl;
            // std::cout << "bucket has hash func: "
            //           << (bucket_hash_func_[index].has_value() ? "YES" :
            //           "NO")
            //           << std::endl;

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

        std::cout << "Hash table" << std::endl;
        for (auto bucket : bucket_hash_table_) {
            for (auto x : bucket) {
                if (x.has_value()) {
                    std::cout << x.value() << ' ';
                }
            }
            std::cout << std::endl;
        }
    }
    bool Contains(int number) const;
};