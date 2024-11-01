#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <random>
#include <set>
#include <unordered_set>
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
        std::cout << "__________________________" << std::endl;
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
        // std::cout << "Mismatch hash: " << first.vertex
        //           << " != " << second.vertex << std::endl;
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

std::random_device rd;
std::mt19937 rng(rd());

int SelectRandom(int first, int last) {
    std::uniform_int_distribution<int> distrib(first, last);
    return distrib(rng);
}

std::pair<Tree, Tree> GenerateRandomTrees(int tree_size) {
    // const int kRandomSeed = 667;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int>(0, tree_size - 1);

    std::vector<int> parent(tree_size, -1);
    for (int index = 1; index < tree_size; ++index) {
        parent[index] = SelectRandom(0, index - 1);
    }

    std::vector<int> permutation(tree_size, 0);
    for (int index = 0; index < tree_size; ++index) {
        permutation[index] = index;
    }
    std::shuffle(permutation.begin(), permutation.end(), rng);

    Tree first_tree(tree_size);
    Tree second_tree(tree_size);

    for (int index = 1; index < tree_size; ++index) {
        int from = parent[index];
        int to = index;

        first_tree.AddEdge({from, to});
        second_tree.AddEdge({permutation[from], permutation[to]});
    }

    return std::make_pair(first_tree, second_tree);
}

bool StressTest(int tree_size);

std::vector<Tree> different_trees;

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int tree_size;
    std::cin >> tree_size;

    const int kEpochCount = 100;

    int correct = 0;
    for (int epoch = 0; epoch < kEpochCount; ++epoch) {
        correct += (StressTest(tree_size) ? 1 : 0);
    }

    std::cout << correct << " correct out of " << kEpochCount << std::endl;
    // std::cout << "different trees: " << different_trees.size() << std::endl;
    // for (auto tree : different_trees) {
    //     tree.PrintEdges();
    // }
}

bool StressTest(int tree_size) {
    auto forest = GenerateRandomTrees(tree_size);

    auto first_tree = forest.first;
    auto second_tree = forest.second;

    // different_trees.push_back(first_tree);
    // different_trees.push_back(second_tree);

    first_tree.CalculateSubtreeSizes();
    second_tree.CalculateSubtreeSizes();

    std::vector<int> first_tree_centroids = first_tree.FindCentroids();
    std::vector<int> second_tree_centroids = second_tree.FindCentroids();

    for (auto first_centroid : first_tree_centroids) {
        for (auto second_centroid : second_tree_centroids) {
            std::vector<int64_t> first_tree_hashs =
                first_tree.CalculateSubtreeHashs(first_centroid);
            std::vector<int64_t> second_tree_hashs =
                second_tree.CalculateSubtreeHashs(second_centroid);

            std::vector<int> vertex_bijection(tree_size, -1);
            bool ok = CheckEquality({first_centroid, -1, first_tree},
                                    {second_centroid, -1, second_tree},
                                    first_tree_hashs, second_tree_hashs,
                                    vertex_bijection);

            if (ok) {
                return true;
            }
        }
    }

    return false;
}