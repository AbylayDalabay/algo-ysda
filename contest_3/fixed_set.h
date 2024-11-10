#include <atomic>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <optional>
#include <random>

class LinearHashFunction {
private:
    int64_t slope_;
    int64_t intercept_;
    int64_t mod_prime_;

public:
    LinearHashFunction() {}
    LinearHashFunction(int64_t slope, int64_t intercept, int64_t mod_prime)
        : slope_(slope), intercept_(intercept), mod_prime_(mod_prime) {
        slope_ %= mod_prime_;
        intercept_ %= mod_prime_;
    }

    int64_t operator()(int64_t x_value) const {
        int64_t hash_value = slope_ * x_value + intercept_;
        hash_value %= mod_prime_;
        if (hash_value < 0) {
            hash_value += mod_prime_;
        }
        return hash_value;
    }
};

int64_t SelectRandom(int64_t first, int64_t last) {
    const int kRandomSeed = 667;
    static std::mt19937_64 random_generator(kRandomSeed);
    std::uniform_int_distribution<int64_t> distrib(first, last - 1);

    return distrib(random_generator);
}

LinearHashFunction GenerateRandomLinearHashFunction() {
    const int64_t kPrimeMod = 87'178'291'199;

    int64_t a_value = SelectRandom(0, kPrimeMod);
    int64_t b_value = SelectRandom(0, kPrimeMod);

    return LinearHashFunction(a_value, b_value, kPrimeMod);
}

template <typename Predicate>
LinearHashFunction GenerateFunctionWithPredicate(Predicate pred) {
    LinearHashFunction current_hash_function;
    do {
        current_hash_function = GenerateRandomLinearHashFunction();
    } while (!pred(current_hash_function));
    return current_hash_function;
}

std::vector<int64_t> CountBucketSizes(const std::vector<int>& elements,
                                      const LinearHashFunction& hash_function,
                                      int64_t hash_table_size) {
    std::vector<int64_t> bucket_sizes(hash_table_size, 0);
    for (auto element : elements) {
        int64_t hash_value = hash_function(element);
        hash_value %= hash_table_size;
        ++bucket_sizes[hash_value];
    }
    return bucket_sizes;
}

std::vector<std::vector<int>> DivideIntoBuckets(
    const std::vector<int>& elements, const LinearHashFunction& hash_function,
    int64_t hash_table_size) {
    std::vector<std::vector<int>> buckets(hash_table_size);
    for (auto element : elements) {
        int64_t hash_value = hash_function(element);
        hash_value %= hash_table_size;
        buckets[hash_value].push_back(element);
    }
    return buckets;
}

int64_t SumOfSquares(const std::vector<int64_t>& array) {
    int64_t sum =
        std::accumulate(array.begin(), array.end(), static_cast<int64_t>(0),
                        [](int64_t sum, int64_t current_value) {
                            return sum + current_value * current_value;
                        });
    return sum;
}

class BucketHashTable {
private:
    std::optional<LinearHashFunction> hash_function_;
    std::vector<std::optional<int>> hash_table_;
    int64_t hash_table_size_;

public:
    BucketHashTable() {}
    void Initialize(const std::vector<int>& elements) {
        if (elements.empty()) {
            hash_function_.reset();
            hash_table_.clear();
            hash_table_size_ = 0;
            return;
        }
        int64_t size = elements.size();
        hash_table_size_ = size * size;

        auto predicate = [&](LinearHashFunction hash_function) -> bool {
            std::vector<int64_t> bucket_sizes =
                CountBucketSizes(elements, hash_function, hash_table_size_);
            int64_t square_sum_buckets = SumOfSquares(bucket_sizes);
            return square_sum_buckets == size;
        };

        hash_function_ = GenerateFunctionWithPredicate(predicate);
        hash_table_.assign(hash_table_size_, std::nullopt);
        for (auto element : elements) {
            int64_t hash_value = hash_function_.value()(element);
            hash_value %= hash_table_size_;
            hash_table_[hash_value] = element;
        }
    }
    bool Contains(int value) const {
        if (!hash_function_.has_value()) {
            return false;
        }
        int64_t hash_value = hash_function_.value()(value);
        hash_value %= hash_table_size_;
        return hash_table_[hash_value] == value;
    }
};

class FixedSet {
private:
    static const int64_t kCoef = 10;
    int64_t hash_table_size_ = 0;
    std::optional<LinearHashFunction> hash_function_;
    std::vector<BucketHashTable> buckets_;

public:
    void Initialize(const std::vector<int>& elements) {
        if (elements.empty()) {
            hash_table_size_ = 0;
            hash_function_.reset();
            buckets_.clear();
            return;
        }
        hash_table_size_ = elements.size();

        auto predicate = [&](LinearHashFunction hash_function) -> bool {
            std::vector<int64_t> bucket_sizes =
                CountBucketSizes(elements, hash_function, hash_table_size_);
            int64_t square_sum_buckets = SumOfSquares(bucket_sizes);
            return square_sum_buckets <= kCoef * hash_table_size_;
        };

        hash_function_ = GenerateFunctionWithPredicate(predicate);

        std::vector<std::vector<int>> bucket_values = DivideIntoBuckets(
            elements, hash_function_.value(), hash_table_size_);

        buckets_.resize(hash_table_size_);
        for (int i = 0; i < hash_table_size_; ++i) {
            buckets_[i].Initialize(bucket_values[i]);
        }
    }
    bool Contains(int value) const {
        if (!hash_function_.has_value()) {
            return false;
        }
        int64_t hash_value = hash_function_.value()(value);
        hash_value %= hash_table_size_;

        const BucketHashTable& current_bucket = buckets_[hash_value];

        return current_bucket.Contains(value);
    }
};