#include <iomanip>
#include <ios>
#include <iostream>
#include <optional>
#include <vector>
#include <cmath>
#include <algorithm>

constexpr double kEpsilon = 1e-12;

struct Segment {
    double left_bound, right_bound;

    Segment(const double& left_bound, const double& right_bound)
        : left_bound(left_bound),
          right_bound(right_bound)
    {}
};

struct Point {
    double x, y;

    std::optional<Segment> IntersectionWithOx(const double& radius) const {

        if (static_cast<double>(fabs(this->y)) > radius) {
            return std::nullopt;
        }

        const double discriminant = radius * radius - this->y * this->y;

        return std::optional<Segment>(
            Segment(
                this->x - sqrt(discriminant), 
                this->x + sqrt(discriminant)
            )            
        );
    }
};

int CountMaxPoints(const std::vector<Point>& points, const double& radius) {

    std::vector<Segment> segments;
    for (const Point& point : points) {
        std::optional<Segment> intersection_segment = point.IntersectionWithOx(radius);

        if (intersection_segment.has_value()) {
            segments.push_back(intersection_segment.value());
        }
    }

    struct SegmentState {
        double coordinate;
        int contribution_value;

        SegmentState(const double& coordinate, const int contribution_value)
            : coordinate(coordinate), contribution_value(contribution_value) {
        }

        bool operator<(const SegmentState& other) const {
            if (fabs(this->coordinate - other.coordinate) <     ) {
                return this->contribution_value > other.contribution_value;
            }
            return this->coordinate < other.coordinate;
        }
    };

    std::vector<SegmentState> segment_states;
    for (const auto& segment : segments) {
        segment_states.push_back(SegmentState(segment.left_bound, 1));
        segment_states.push_back(SegmentState(segment.right_bound, -1));
    }

    sort(begin(segment_states), end(segment_states));

    int current_intersection_count = 0;
    int max_intersection_count = 0;

    for (const SegmentState& state : segment_states) {
        current_intersection_count += state.contribution_value;
        max_intersection_count = std::max(max_intersection_count, current_intersection_count);
    }

    return max_intersection_count;
}

int main() {
    int n, k;
    std::cin >> n >> k;

    std::vector<Point> points(n);
    for (auto& point : points) {
        std::cin >> point.x >> point.y;
    }

    double left_radius_bound = 0;
    double right_radius_bound = 2000;

    while (right_radius_bound - left_radius_bound > kEpsilon) {

        double mid_radius = (left_radius_bound + right_radius_bound) / 2;

        if (CountMaxPoints(points, mid_radius) < k) {
            left_radius_bound = mid_radius;
        } else {
            right_radius_bound = mid_radius;
        }
    }

    std::cout << std::fixed << std::setprecision(6) << right_radius_bound << std::endl;

    return 0;
}