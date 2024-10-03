#include <algorithm>
#include <iostream>
#include <vector>

struct Point {
    int x, expiration_time;

    bool operator<(const Point& other) const { return this->x < other.x; }
};

struct SegmentTime {
    static constexpr int kInf = 2e9 + 500;

    int left_index_time;
    int right_index_time;

    SegmentTime() : left_index_time(kInf), right_index_time(kInf) {}
    SegmentTime(int left_index_time, int right_index_time)
        : left_index_time(left_index_time),
          right_index_time(right_index_time) {}

    void UpdateIfPossible();
};

int array_size;
std::vector<Point> array;
std::vector<std::vector<SegmentTime>> dp;

int GetDistance(int start_index, int finish_index) {
    return std::abs(array[start_index].x - array[finish_index].x);
}

void UpdateDp(int left_index, int right_index) {
    SegmentTime updated_segment_time = SegmentTime();

    // 1) end_l = [l, r - 1].r + [r - 1, r] + [l, r]
    // 2) end_l = [l + 1, r].l + [l, l + 1]
    // 3) end_l = [l + 1, r].r + [l, r]
    updated_segment_time.left_index_time =
        std::min({dp[left_index][right_index - 1].right_index_time +
                      GetDistance(right_index - 1, right_index) +
                      GetDistance(right_index, left_index),
                  dp[left_index + 1][right_index].left_index_time +
                      GetDistance(left_index + 1, left_index),
                  dp[left_index + 1][right_index].right_index_time +
                      GetDistance(left_index, right_index)});
    // check if not expired time
    if (updated_segment_time.left_index_time >
        array[left_index].expiration_time) {
        updated_segment_time.left_index_time = SegmentTime::kInf;
    }

    // 1) end_r = [l, r - 1].r + [r - 1, r]
    // 2) end_r = [l + 1, r].l + [l, l + 1] + [l, r]
    // 3) end_r = [l, r - 1].l + [l, r] ?
    updated_segment_time.right_index_time =
        std::min({dp[left_index][right_index - 1].right_index_time +
                      GetDistance(right_index - 1, right_index),
                  dp[left_index + 1][right_index].left_index_time +
                      GetDistance(left_index, left_index + 1) +
                      GetDistance(left_index, right_index),
                  dp[left_index][right_index - 1].left_index_time +
                      GetDistance(left_index, right_index)});
    // check if not expired time
    if (updated_segment_time.right_index_time >
        array[right_index].expiration_time) {
        updated_segment_time.right_index_time = SegmentTime::kInf;
    }

    updated_segment_time.left_index_time =
        std::min(updated_segment_time.left_index_time,
                 updated_segment_time.right_index_time +
                     GetDistance(left_index, right_index));

    updated_segment_time.right_index_time =
        std::min(updated_segment_time.right_index_time,
                 updated_segment_time.left_index_time +
                     GetDistance(left_index, right_index));

    if (updated_segment_time.left_index_time <
        dp[left_index][right_index].left_index_time) {
        dp[left_index][right_index].left_index_time =
            updated_segment_time.left_index_time;
    }
    if (updated_segment_time.right_index_time <
        dp[left_index][right_index].right_index_time) {
        dp[left_index][right_index].right_index_time =
            updated_segment_time.right_index_time;
    }
}

int main() {
    std::cin >> array_size;
    array.resize(array_size);
    for (auto& point : array) {
        std::cin >> point.x >> point.expiration_time;
    }

    std::sort(array.begin(), array.end());

    dp.assign(array_size, std::vector<SegmentTime>(array_size, SegmentTime()));

    for (int index = 0; index < array_size; ++index) {
        dp[index][index] = SegmentTime(0, 0);
    }

    for (int index = 0; index + 1 < array_size; ++index) {
        const int kCurrentDist = GetDistance(index, index + 1);

        dp[index][index + 1].left_index_time =
            (kCurrentDist <= array[index].expiration_time ? kCurrentDist
                                                          : SegmentTime::kInf);
        dp[index][index + 1].right_index_time =
            (kCurrentDist <= array[index + 1].expiration_time
                 ? kCurrentDist
                 : SegmentTime::kInf);
    }

    for (int len = 3; len <= array_size; ++len) {
        int left_index = 0;
        int right_index = len - 1;
        while (right_index < array_size) {
            UpdateDp(left_index, right_index);
            ++left_index;
            ++right_index;
        }
    }

    int min_time = std::min(dp[0][array_size - 1].left_index_time,
                            dp[0][array_size - 1].right_index_time);

    if (min_time >= SegmentTime::kInf) {
        std::cout << "No solution";
    } else {
        std::cout << min_time;
    }

    return 0;
}