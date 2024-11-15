#include <iostream>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::shared_ptr<Node> left_child;
    std::shared_ptr<Node> right_child;

    Node(int value) : value(value), left_child(nullptr), right_child(nullptr) {}
};

template <typename Iterator>
std::shared_ptr<Node> ConstructBST(const Iterator& begin,
                                   const Iterator& end) {
    if (begin == end) {
        return nullptr;
    }
    if (begin + 1 == end) {
        return std::shared_ptr<Node>(new Node(*begin));
    }

    Iterator left_begin = end;
    Iterator left_end = end;
    Iterator right_begin = end;
    Iterator right_end = end;

    for (Iterator current_it = begin; current_it != end; ++current_it) {
        
    }
}

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }
}