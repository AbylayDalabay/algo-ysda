#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <random>
#include <vector>

class CustomHash {
private:
    static const uint64_t k30 = 30;
    static const uint64_t k27 = 27;
    static const uint64_t k31 = 31;

    static const uint64_t kFirst = 0x9e3779b97f4a7c15;
    static const uint64_t kSecond = 0xbf58476d1ce4e5b9;
    static const uint64_t kThird = 0x94d049bb133111eb;

    static uint64_t Splitmix64(uint64_t value) {
        value += kFirst;
        value = (value ^ (value >> k30)) * kSecond;
        value = (value ^ (value >> k27)) * kThird;
        return value ^ (value >> k31);
    }

public:
    size_t operator()(uint64_t value) const {
        static const uint64_t kFixedRandom =
            std::chrono::steady_clock::now().time_since_epoch().count();
        return Splitmix64(value + kFixedRandom);
    }
};

struct Edge {
    int from, to;
};

class Tree {
private:
    int size_;

    std::vector<Edge> edges_;
    std::vector<std::vector<int>> graph_;
    std::vector<int> subtree_size_;

    static const int64_t kPrime = 1e9 + 9;

    void CalculateSubtreeSizes(int vertex, int parent_vertex) {
        subtree_size_[vertex] = 1;
        for (int to_vertex : graph_[vertex]) {
            if (to_vertex != parent_vertex) {
                CalculateSubtreeSizes(to_vertex, vertex);
                subtree_size_[vertex] += subtree_size_[to_vertex];
            }
        }
    }

    void FindCentroids(int vertex, int parent_vertex,
                       std::vector<int>& centroids) {
        bool is_centroid = true;
        for (int to_vertex : graph_[vertex]) {
            if (to_vertex != parent_vertex) {
                FindCentroids(to_vertex, vertex, centroids);
                if (2 * subtree_size_[to_vertex] > size_) {
                    is_centroid = false;
                }
            }
        }
        int other_size = size_ - subtree_size_[vertex];
        if (2 * other_size > size_) {
            is_centroid = false;
        }
        if (is_centroid) {
            centroids.push_back(vertex);
        }
    }

    void CalculateSubtreeHashs(int vertex, int parent_vertex,
                               std::vector<int64_t>& hashs) {
        for (auto to_vertex : graph_[vertex]) {
            if (to_vertex != parent_vertex) {
                CalculateSubtreeHashs(to_vertex, vertex, hashs);
            }
        }

        std::sort(graph_[vertex].begin(), graph_[vertex].end(),
                  [&](int lhs, int rhs) { return hashs[lhs] < hashs[rhs]; });

        int64_t current_subtree_hash = 1;
        for (auto to_vertex : graph_[vertex]) {
            current_subtree_hash =
                (current_subtree_hash * kPrime + hashs[to_vertex]);
        }
        hashs[vertex] = current_subtree_hash;
    }

public:
    Tree(int size) : size_(size) {
        graph_.resize(size);
        subtree_size_.assign(size_, 0);
    }

    void AddEdge(Edge edge) {
        edges_.push_back(edge);
        graph_[edge.from].push_back(edge.to);
        graph_[edge.to].push_back(edge.from);
    }

    void CalculateSubtreeSizes() { CalculateSubtreeSizes(0, -1); }

    std::vector<int64_t> CalculateSubtreeHashs(int vertex) {
        std::vector<int64_t> hashs(size_, 0);
        CalculateSubtreeHashs(vertex, -1, hashs);
        return hashs;
    }

    std::vector<int> FindCentroids() {
        std::vector<int> centroids;
        FindCentroids(0, -1, centroids);
        return centroids;
    }

    const std::vector<int>& Childrens(int vertex) const {
        return graph_[vertex];
    }

    void PrintEdges() const {
        std::cout << "Edges" << std::endl;
        for (const Edge& edge : edges_) {
            std::cout << edge.from << ' ' << edge.to << std::endl;
        }
        std::cout << std::endl;
    }
};

struct VertexState {
    int vertex;
    int parent;

    const Tree& tree;
};

bool CheckEquality(VertexState first, VertexState second,
                   const std::vector<int64_t>& first_hashs,
                   const std::vector<int64_t>& second_hashs,
                   std::vector<int>& vertex_bijection) {
    if (first_hashs[first.vertex] != second_hashs[second.vertex]) {
        std::cout << "Mismatch hash: " << first.vertex
                  << " != " << second.vertex << std::endl;
        return false;
    }

    const std::vector<int>& first_childrens =
        first.tree.Childrens(first.vertex);
    const std::vector<int>& second_childrens =
        second.tree.Childrens(second.vertex);

    if (first_childrens.size() != second_childrens.size()) {
        std::cout << "Mismatch size: " << first.vertex
                  << " != " << second.vertex << std::endl;
        return false;
    }

    vertex_bijection[first.vertex] = second.vertex;

    bool is_correct = true;
    for (int index = 0; index < first_childrens.size(); ++index) {
        int first_to = first_childrens[index];
        int second_to = second_childrens[index];

        if (first_to != first.parent && second_to != second.parent) {
            is_correct &=
                CheckEquality({first_to, first.vertex, first.tree},
                              {second_to, second.vertex, second.tree},
                              first_hashs, second_hashs, vertex_bijection);
        }
    }

    return is_correct;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int tree_size;
    std::cin >> tree_size;

    const int kRandomSeed = 668;

    std::vector<int> permutation(tree_size, kRandomSeed);
    for (int index = 0; index < tree_size; ++index) {
        permutation[index] = index;
    }

    std::mt19937 generator(kRandomSeed);
    std::shuffle(permutation.begin(), permutation.end(), generator);

    std::cout << "permutation: " << std::endl;
    for (int index = 0; index < tree_size; ++index) {
        std::cout << permutation[index] << ' ';
    }
    std::cout << std::endl;

    Tree first_tree(tree_size);
    Tree second_tree(tree_size);
    for (int index = 0; index < tree_size - 1; ++index) {
        int from;
        int to;
        std::cin >> from >> to;
        --from;
        --to;
        first_tree.AddEdge({from, to});

        second_tree.AddEdge({permutation[from], permutation[to]});
    }

    // for (int index = 0; index < tree_size - 1; ++index) {
    //     int from;
    //     int to;
    //     std::cin >> from >> to;
    //     --from;
    //     --to;

    //     std::cout << from << " => " << permutation[from] << std::endl;
    //     std::cout << to << " => " << permutation[to] << std::endl;

    //     from = permutation[from];
    //     to = permutation[to];
    //     second_tree.AddEdge({from, to});
    // }

    first_tree.PrintEdges();

    second_tree.PrintEdges();

    first_tree.CalculateSubtreeSizes();
    second_tree.CalculateSubtreeSizes();

    std::vector<int> first_tree_centroids = first_tree.FindCentroids();
    std::vector<int> second_tree_centroids = second_tree.FindCentroids();

    std::cout << "First Tree centroids" << std::endl;
    for (auto x : first_tree_centroids) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    std::cout << "Second Tree centroids" << std::endl;
    for (auto x : second_tree_centroids) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    std::vector<int64_t> first_tree_hashs = first_tree.CalculateSubtreeHashs(2);
    std::vector<int64_t> second_tree_hashs =
        second_tree.CalculateSubtreeHashs(0);

    std::cout << "first_tree_hashs" << std::endl;
    for (int index = 0; index < first_tree_hashs.size(); ++index) {
        std::cout << index << ": " << first_tree_hashs[index] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "second_tree_hashs" << std::endl;
    for (int index = 0; index < second_tree_hashs.size(); ++index) {
        std::cout << index << ": " << second_tree_hashs[index] << std::endl;
    }
    std::cout << std::endl;

    std::vector<int> vertex_bijection(tree_size, -1);
    bool ok =
        CheckEquality({2, -1, first_tree}, {0, -1, second_tree},
                      first_tree_hashs, second_tree_hashs, vertex_bijection);

    std::cout << (ok ? "Correct" : "Not correct") << std::endl;
    std::cout << "bijection: " << std::endl;
    for (int index = 0; index < tree_size; ++index) {
        std::cout << index << " -> " << vertex_bijection[index] << std::endl;
    }
}