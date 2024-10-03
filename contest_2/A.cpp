#include <iostream>
#include <limits>
#include <stack>
#include <vector>

struct Node {
    int value, pref_max;

    Node(int value, int pref_max) : value(value), pref_max(pref_max) {}
};

struct MaxStack {
    std::stack<Node> max_stack;

    bool Empty() const { return max_stack.empty(); }
    void Push(int value) {
        Node current_node = Node(value, value);
        if (!max_stack.empty()) {
            current_node.pref_max = std::max(max_stack.top().pref_max, value);
        }
        max_stack.push(current_node);
    }

    int GetMax() const { return max_stack.top().pref_max; }

    int Top() const { return max_stack.top().value; }

    void Pop() { return max_stack.pop(); }
};

struct MaxQueue {
    MaxStack back;
    MaxStack front;
    static const int kMin = std::numeric_limits<int>::min();

    bool Empty() const { return front.Empty() && back.Empty(); }

    void PushBack(int value) { back.Push(value); }

    void PopFront() {
        if (front.Empty()) {
            while (!back.Empty()) {
                front.Push(back.Top());
                back.Pop();
            }
        }
        front.Pop();
    }

    int GetMax() const {
        int current_max = kMin;
        if (!back.Empty()) {
            current_max = std::max(current_max, back.GetMax());
        }
        if (!front.Empty()) {
            current_max = std::max(current_max, front.GetMax());
        }
        return current_max;
    }
};

int main() {
    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size, 0);
    for (auto& x : array) {
        std::cin >> x;
    }

    MaxQueue max_queue;

    int right_index = 0;
    max_queue.PushBack(array[0]);

    int queries;
    std::cin >> queries;
    for (int query = 0; query < queries; ++query) {
        char direction;
        std::cin >> direction;

        if (direction == 'L') {
            max_queue.PopFront();
        } else {
            ++right_index;
            max_queue.PushBack(array[right_index]);
        }

        std::cout << max_queue.GetMax() << ' ';
    }
    std::cout << std::endl;

    return 0;
}