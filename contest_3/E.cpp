#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>
#include <vector>

class Triangle {
private:
    std::vector<int> segments_;

public:
    Triangle(std::vector<int> segments) : segments_(segments) {}

    void Normalize() {
        int gcd = 0;
        for (int segment : segments_) {
            gcd = std::gcd(gcd, segment);
        }
        std::sort(segments_.begin(), segments_.end());
        for (int& segment : segments_) {
            segment /= gcd;
        }
    }

    bool operator<(const Triangle& other) const {
        return this->segments_ < other.segments_;
    }

    bool operator==(const Triangle& other) const {
        return this->segments_ == other.segments_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Triangle& triangle);
};

std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
    for (const auto& segment : triangle.segments_) {
        os << segment << ' ';
    }
    return os;
}

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<Triangle> array;
    for (int index = 0; index < array_size; ++index) {
        int x;
        int y;
        int z;
        std::cin >> x >> y >> z;
        Triangle triangle = Triangle({x, y, z});
        triangle.Normalize();
        array.push_back(triangle);
    }

    std::sort(array.begin(), array.end());

    // for (const auto& triangle : array) {
    //     std::cout << triangle << std::endl;
    // }

    std::vector<Triangle> unique_array;
    unique_array.push_back(*array.begin());

    for (const auto& triangle : array) {
        if (triangle == unique_array.back()) {
            continue;
        }
        unique_array.push_back(triangle);
    }

    std::cout << unique_array.size() << std::endl;
}