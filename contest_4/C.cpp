#include <cassert>
#include <ios>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::shared_ptr<Node> left_child;
    std::shared_ptr<Node> right_child;

    Node(int value) : value(value), left_child(nullptr), right_child(nullptr) {}

    Node(int value, std::shared_ptr<Node> left_child,
         std::shared_ptr<Node> right_child)
        : value(value), left_child(left_child), right_child(right_child) {}
};

template <typename Iterator>
std::shared_ptr<Node> ConstructBST(const Iterator& begin, const Iterator& end) {
    if (begin == end) {
        return nullptr;
    }

    if (begin + 1 == end) {
        return std::shared_ptr<Node>(new Node(*begin));
    }

    Iterator left_begin = std::next(begin);
    Iterator left_end = std::next(begin);
    Iterator right_begin = std::next(begin);
    Iterator right_end = end;

    int root_value = *begin;

    Iterator left_it = begin;
    Iterator right_it = end;

    while (left_it + 1 < right_it) {
        int dist = std::distance(left_it, right_it);
        Iterator mid_it = left_it + dist / 2;

        if (*mid_it < root_value) {
            left_it = mid_it;
            left_end = std::next(mid_it);
            right_begin = std::next(mid_it);
        } else {
            right_it = mid_it;
        }
    }

    std::shared_ptr<Node> left_bst = ConstructBST(left_begin, left_end);
    std::shared_ptr<Node> right_bst = ConstructBST(right_begin, right_end);

    return std::shared_ptr<Node>(new Node(root_value, left_bst, right_bst));
}

void InorderOrderTraverse(const std::shared_ptr<Node>& ptr) {
    if (ptr == nullptr) {
        return;
    }

    InorderOrderTraverse(ptr->left_child);

    std::cout << ptr->value << " ";

    InorderOrderTraverse(ptr->right_child);
}

void PostOrderTraverse(const std::shared_ptr<Node>& ptr) {
    if (ptr == nullptr) {
        return;
    }

    PostOrderTraverse(ptr->left_child);
    PostOrderTraverse(ptr->right_child);

    std::cout << ptr->value << " ";
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    auto root = ConstructBST(array.begin(), array.end());

    PostOrderTraverse(root);
    std::cout << std::endl;

    InorderOrderTraverse(root);
    std::cout << std::endl;
}