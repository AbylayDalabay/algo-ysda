#include <algorithm>
#include <iostream>
#include <list>
#include <numeric>
#include <ostream>
#include <vector>

class Triangle {
private:
    std::vector<int> sides_;

public:
    Triangle(int x_side, int y_side, int z_side)
        : sides_({x_side, y_side, z_side}) {}

    bool operator==(const Triangle& triangle) const {
        return this->sides_ == triangle.sides_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Triangle& triangle);

    void Normalize() {
        int gcd = sides_[0];
        for (auto x : sides_) {
            gcd = std::gcd(gcd, x);
        }
        for (auto& x : sides_) {
            x /= gcd;
        }
        sort(sides_.begin(), sides_.end());
    }

    static const int64_t kPrime = 1e9 + 7;
    int64_t Hash() const {
        int64_t hash_sum = 0;
        hash_sum += (sides_[0] * sides_[1]) % kPrime;
        hash_sum += (sides_[1] * sides_[2]) % kPrime;
        hash_sum %= kPrime;
        return hash_sum;
    }
};

std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
    os << "{";
    for (auto side : triangle.sides_) {
        os << side << ", ";
    }
    os << "}";
    return os;
}

template <typename T>
class HashTable {
private:
    int size_;
    int unique_size_;
    std::vector<std::list<T>> hash_table_;

public:
    HashTable<T>(int size) : size_(size) { hash_table_.resize(size); }

    int HashIndex(const T& value) const {
        int hash_index = value.Hash() % size_;
        if (hash_index < 0) {
            hash_index += size_;
        }
        return hash_index % size_;
    }

    bool Contains(const T& value) {
        int hash_index = HashIndex(value);
        return std::any_of(
            hash_table_[hash_index].begin(), hash_table_[hash_index].end(),
            [value](const Triangle& elem) { return elem == value; });
    }

    void Insert(const T& value) {
        if (!Contains(value)) {
            int hash_index = HashIndex(value);
            hash_table_[hash_index].push_back(value);
            ++unique_size_;
        }
    }

    void PrintBuckets() const {
        for (int index = 0; index < size_; ++index) {
            std::cout << "Bucket " << index << " : ";
            for (const Triangle& triangle : hash_table_[index]) {
                std::cout << triangle << ' ';
            }
            std::cout << std::endl;
        }
    }
    int Size() const { return unique_size_; }
};

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<Triangle> triangles;
    triangles.reserve(array_size);
    for (int index = 0; index < array_size; ++index) {
        int x_side;
        int y_side;
        int z_side;
        std::cin >> x_side >> y_side >> z_side;
        triangles.emplace_back(x_side, y_side, z_side);
    }

    for (auto& triangle : triangles) {
        triangle.Normalize();
    }

    HashTable<Triangle> hash_table(array_size);

    for (const auto& triangle : triangles) {
        hash_table.Insert(triangle);
    }

    std::cout << hash_table.Size() << std::endl;
}