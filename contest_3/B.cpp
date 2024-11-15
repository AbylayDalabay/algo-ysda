#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>

struct Edge {
    int from, to;

    bool operator==(const Edge& other) const {
        return (from == other.from) && (to == other.to);
    }

    bool operator<(const Edge& other) const {
        if (from != other.from) {
            return from < other.from;
        }
        return to < other.to;
    }
};

class Tree {
private:
    int size_;

    std::vector<Edge> edges_;
    std::vector<std::vector<int>> graph_;
    std::vector<int> subtree_size_;

    constexpr static const double kStartingHashValue = 69;

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
                               std::vector<double>& hashs) {
        if (graph_[vertex].empty()) {
            hashs[vertex] = kStartingHashValue;
            return;
        }
        for (auto to_vertex : graph_[vertex]) {
            if (to_vertex != parent_vertex) {
                CalculateSubtreeHashs(to_vertex, vertex, hashs);
            }
        }

        // std::cout << "current hashes" << std::endl;
        // for (auto gg : hashs) {
        //     std::cout << gg << std::endl;
        // }
        // std::cout << std::endl;

        std::sort(graph_[vertex].begin(), graph_[vertex].end(),
                  [&](int lhs, int rhs) { return hashs[lhs] < hashs[rhs]; });

        double current_subtree_hash = kStartingHashValue;
        for (auto to_vertex : graph_[vertex]) {
            current_subtree_hash += log(hashs[to_vertex]);
        }
        hashs[vertex] = current_subtree_hash;
    }

public:
    Tree(int size) : size_(size) {
        graph_.resize(size);
        subtree_size_.assign(size_, 0);
    }

    int Size() const { return size_; }

    void AddEdge(Edge edge) {
        if (edge.from > edge.to) {
            std::swap(edge.from, edge.to);
        }
        edges_.push_back(edge);
        graph_[edge.from].push_back(edge.to);
        graph_[edge.to].push_back(edge.from);
    }

    void CalculateSubtreeSizes() {
        subtree_size_.assign(size_, 0);
        CalculateSubtreeSizes(0, -1);
    }

    std::vector<double> CalculateSubtreeHashs(int vertex) {
        std::vector<double> hashs(size_, kStartingHashValue);
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

    std::vector<Edge> GetEdges() const { return edges_; }
};

struct VertexState {
    int vertex;
    int parent;

    const Tree& tree;
};

bool CheckEquality(VertexState first, VertexState second,
                   const std::vector<double>& first_hashs,
                   const std::vector<double>& second_hashs,
                   std::vector<int>& vertex_bijection) {
    // if (first_hashs[first.vertex] != second_hashs[second.vertex]) {
    //     return false;
    // }

    const std::vector<int>& first_childrens =
        first.tree.Childrens(first.vertex);
    const std::vector<int>& second_childrens =
        second.tree.Childrens(second.vertex);

    if (first_childrens.size() != second_childrens.size()) {
        return false;
    }

    vertex_bijection[first.vertex] = second.vertex;

    bool is_correct = true;
    for (int index = 0; index < static_cast<int>(first_childrens.size());
         ++index) {
        int first_to = first_childrens[index];
        int second_to = second_childrens[index];

        if (first_to != first.parent && second_to != second.parent) {
            is_correct &=
                CheckEquality({first_to, first.vertex, first.tree},
                              {second_to, second.vertex, second.tree},
                              first_hashs, second_hashs, vertex_bijection);
        }

        if (!is_correct) {
            return false;
        }
    }

    return is_correct;
}

bool EqualEdges(std::vector<Tree>& trees,
                const std::vector<int>& vertex_bijection) {
    std::vector<std::vector<Edge>> edges(2);
    for (int index = 0; index < 2; ++index) {
        edges[index] = trees[index].GetEdges();
    }

    for (Edge& edge : edges[0]) {
        edge.from = vertex_bijection[edge.from];
        edge.to = vertex_bijection[edge.to];
        if (edge.from > edge.to) {
            std::swap(edge.from, edge.to);
        }
    }

    for (int index = 0; index < 2; ++index) {
        sort(edges[index].begin(), edges[index].end());
        // std::cout << "Edges: " << index << std::endl;
        // for (const auto& edge : edges[index]) {
        //     std::cout << "{" << edge.from << ", " << edge.to << "}"
        //               << std::endl;
        // }
        // std::cout << "_______________________" << std::endl;
    }

    for (const Edge& target_edge : edges[0]) {
        bool found =
            std::binary_search(edges[1].begin(), edges[1].end(), target_edge);
        if (!found) {
            return false;
        }
    }

    return true;
}

void CheckIsomorphism(std::vector<Tree>& trees,
                      std::vector<std::vector<int>>& centroids) {
    int tree_size = trees[0].Size();
    for (int first_centroid : centroids[0]) {
        for (int second_centroid : centroids[1]) {
            std::vector<double> first_tree_hashs =
                trees[0].CalculateSubtreeHashs(first_centroid);
            std::vector<double> second_tree_hashs =
                trees[1].CalculateSubtreeHashs(second_centroid);

            // std::cout << "first_tree_hashs: " << std::endl;
            // for (auto current_hash_value : first_tree_hashs) {
            //     std::cout << current_hash_value << ' ';
            // }
            // std::cout << std::endl;
            // std::cout << "second_tree_hashs: " << std::endl;
            // for (auto current_hash_value : second_tree_hashs) {
            //     std::cout << current_hash_value << ' ';
            // }
            // std::cout << std::endl;

            std::vector<int> vertex_bijection(tree_size, -1);
            bool ok = CheckEquality(
                {first_centroid, -1, trees[0]}, {second_centroid, -1, trees[1]},
                first_tree_hashs, second_tree_hashs, vertex_bijection);

            if (ok) {
                assert(EqualEdges(trees, vertex_bijection));
                for (int index = 0; index < tree_size; ++index) {
                    std::cout << vertex_bijection[index] + 1 << std::endl;
                }
                exit(0);
            }
        }
    }
}

int main() {
    int tree_size;
    std::cin >> tree_size;

    std::vector<Tree> trees(2, Tree(tree_size));
    for (Tree& current_tree : trees) {
        for (int index = 0; index < tree_size - 1; ++index) {
            int from;
            int to;
            std::cin >> from >> to;
            --from;
            --to;
            current_tree.AddEdge({from, to});
        }
    }

    std::vector<std::vector<int>> centroids(2);
    for (int index = 0; index < 2; ++index) {
        trees[index].CalculateSubtreeSizes();
        centroids[index] = trees[index].FindCentroids();
    }

    CheckIsomorphism(trees, centroids);

    std::cout << -1 << std::endl;

    return 0;
}