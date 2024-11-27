#include <iostream>
#include <vector>

struct Rectangle {
    int left_x;
    int left_y;
    int right_x;
    int right_y;
    int color;
    int id;
};

struct State {
    int coordinate;
    int color;
    int id;
    bool is_add;
};

int main() {
    int max_x;
    int max_y;
    int size;
    std::cin >> max_x >> max_y >> size;

    std::vector<Rectangle> rectangles;
    rectangles.reserve(size);
    for (int index = 2; index <= size + 1; ++index) {
        Rectangle rect;
        std::cin >> rect.left_x >> rect.left_y;
        std::cin >> rect.right_x >> rect.right_y;
        std::cin >> rect.color;
        rect.id = index;
        rectangles.emplace_back(rect);
    }
}