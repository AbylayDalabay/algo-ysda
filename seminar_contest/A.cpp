#include <cstdint>
#include <iostream>
#include <limits>
#include <stack>
#include <vector>

template <typename T>
struct Node {
    T value, pref_max;

    Node(T value, T pref_max) : value(value), pref_max(pref_max) {}
};

template <typename T>
class MaxStack {
private:
    std::stack<Node<T>> max_stack_;

public:
    bool Empty() const { return max_stack_.empty(); }
    int Size() const { return max_stack_.size(); }
    void Push(T value) {
        Node current_node = Node(value, value);
        if (!max_stack_.empty()) {
            current_node.pref_max = std::max(max_stack_.top().pref_max, value);
        }
        max_stack_.push(current_node);
    }

    T GetMax() const { return max_stack_.top().pref_max; }

    T Top() const { return max_stack_.top().value; }

    void Pop() { return max_stack_.pop(); }
};

template <typename T>
class MaxQueue {
private:
    static const T kMin = std::numeric_limits<T>::min();

    MaxStack<T> back_;
    MaxStack<T> front_;

    T added_value_ = 0;

public:
    bool Empty() const { return front_.Empty() && back_.Empty(); }

    int Size() const { return front_.Size() + back_.Size(); }

    void IncrementAll(T value) {
        if (!Empty()) {
            added_value_ += value;
        }
    }

    void PushBack(T value) { back_.Push(value - added_value_); }

    void PopFront() {
        if (front_.Empty()) {
            while (!back_.Empty()) {
                front_.Push(back_.Top());
                back_.Pop();
            }
        }
        front_.Pop();
    }

    T GetMax() const {
        if (Empty()) {
            return kMin;
        }
        T current_max = kMin;
        if (!back_.Empty()) {
            current_max = std::max(current_max, back_.GetMax());
        }
        if (!front_.Empty()) {
            current_max = std::max(current_max, front_.GetMax());
        }
        return current_max + added_value_;
    }
};

struct Group {
    int64_t count;
    int64_t weight;
    int64_t cost;
};

int main() {
    // int group_count;
    // int64_t max_weight;
    // std::cin >> group_count >> max_weight;

    // std::vector<Group> groups(group_count);
    // for (Group& current_group : groups) {
    //     std::cin >> current_group.weight;
    //     std::cin >> current_group.cost;
    //     std::cin >> current_group.count;
    //     current_group.count = std::min(current_group.count, max_weight);
    // }

    // std::cout << "kot" << std::endl;

    // const int64_t kMin = std::numeric_limits<int64_t>::min();
    // const int kOtak = 20;

    // std::vector<int64_t> last_dp(max_weight + 1, 0);

    // std::vector<int64_t> current_dp;

    // for (const Group& group : groups) {
    //     current_dp.assign(max_weight + 1, kMin);
    //     current_dp[0] = 0;

    //     for (int start_weight = 0; start_weight < group.weight;
    //          ++start_weight) {
    //         MaxQueue<int64_t> max_queue;
    //         std::cout << "Start weight: " << start_weight << std::endl;

    //         for (int current_weight = start_weight;
    //              current_weight <= max_weight; current_weight += group.weight) {
    //             max_queue.IncrementAll(group.cost);
    //             max_queue.PushBack(last_dp[current_weight]);

    //             while (max_queue.Size() > group_count) {
    //                 max_queue.PopFront();
    //             }

    //             current_dp[current_weight] = max_queue.GetMax();
    //         }

    //         std::cout << "-------current_dp after start weight-------"
    //                   << std::endl;
    //         for (int index = 0; index < kOtak; ++index) {
    //             std::cout << current_dp[index] << ' ';
    //         }
    //         std::cout << std::endl;
    //     }

    //     last_dp = std::move(current_dp);

    //     std::cout << "-------last_dp-------" << std::endl;
    //     for (int index = 0; index < kOtak; ++index) {
    //         std::cout << last_dp[index] << ' ';
    //     }
    //     std::cout << std::endl;
    // }



    MaxQueue<int> max_queue;

    max_queue.PushBack(1);

    std::cout << max_queue.GetMax() << std::endl;

    max_queue.IncrementAll(5);

    std::cout << max_queue.GetMax() << std::endl;
}
