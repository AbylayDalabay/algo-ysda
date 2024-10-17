#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <iterator>
#include <ostream>
#include <random>
#include <vector>

struct Player {
    int64_t score;
    int index;

    static bool CompByScore(const Player& lhs, const Player& rhs) {
        return lhs.score < rhs.score;
    }
    static bool CompByIndex(const Player& lhs, const Player& rhs) {
        return lhs.index < rhs.index;
    }

    bool operator==(const Player& other) const {
        return this->score == other.score && this->index == other.index;
    }
};

template <typename Iterator, typename UnaryPred>
Iterator Partition(Iterator first, Iterator last, UnaryPred pred) {
    if (std::distance(first, last) <= 1) {
        return first;
    }
    Iterator split_it = first;
    for (Iterator current_it = first; current_it != last; ++current_it) {
        if (pred(*current_it)) {
            std::iter_swap(split_it, current_it);
            ++split_it;
        }
    }
    return split_it;
}

template <typename Iterator>
Iterator SelectPivot(Iterator first, Iterator last) {
    int distance = std::distance(first, last);
    if (distance <= 0) {
        assert(false);
    }

    const int kRandomSeed = 667;
    std::mt19937 rng(kRandomSeed);
    std::uniform_int_distribution<int> distrib(0, distance - 1);

    const int kPivotIndex = distrib(rng);
    return first + kPivotIndex;
}

template <typename Iterator, typename Comparator>
void QuickSort(Iterator first, Iterator last, Comparator comparator) {
    if (std::distance(first, last) <= 1) {
        return;
    }

    const Iterator kPivotIterator = SelectPivot(first, last);

    const Iterator kEqualIterator =
        Partition(first, last, [kPivotIterator, comparator](const auto& elem) {
            return comparator(elem, *kPivotIterator);
        });

    const Iterator kGreaterIterator = Partition(
        kEqualIterator, last, [kPivotIterator, comparator](const auto& elem) {
            return !comparator(*kPivotIterator, elem);
        });

    QuickSort(first, kEqualIterator, comparator);
    QuickSort(kGreaterIterator, last, comparator);
}

template <typename Iterator>
std::vector<Player> BuildMostEffectiveSolidaryTeam(Iterator first,
                                                   Iterator last) {
    auto is_solidary = [](Iterator first, Iterator last) -> bool {
        if (std::distance(first, last) <= 1) {
            return true;
        }
        return first->score + std::next(first)->score >= last->score;
    };

    int64_t best_summary_score = 0;
    int64_t current_summary_score = 0;

    Iterator best_first = first;
    Iterator best_last = first;

    Iterator current_first = first;
    for (Iterator current_last = first; current_last != last; ++current_last) {
        current_summary_score += current_last->score;

        while (current_first < current_last &&
               !is_solidary(current_first, current_last)) {
            current_summary_score -= current_first->score;
            ++current_first;
        }

        if (current_summary_score >= best_summary_score) {
            best_summary_score = current_summary_score;
            best_first = current_first;
            best_last = current_last;
        }
    }

    std::vector<Player> answer;
    for (auto current_it = best_first; current_it <= best_last; ++current_it) {
        answer.push_back(*current_it);
    }

    return answer;
}

std::vector<Player> InputPlayersVector() {
    int players_size;
    std::cin >> players_size;
    std::vector<Player> players(players_size);
    for (int current_index = 0; current_index < players_size; ++current_index) {
        std::cin >> players[current_index].score;
        players[current_index].index = current_index + 1;
    }
    return players;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Player> players = InputPlayersVector();

    QuickSort(players.begin(), players.end(), Player::CompByScore);

    std::vector<Player> best_players =
        BuildMostEffectiveSolidaryTeam(players.begin(), players.end());

    int64_t summary_score = 0;
    for (const auto& player : best_players) {
        summary_score += player.score;
    }

    QuickSort(best_players.begin(), best_players.end(), Player::CompByIndex);

    bool ok = true;
    for (int index = 0; index + 1 < static_cast<int>(best_players.size());
         ++index) {
        ok &= (best_players[index].index < best_players[index + 1].index);
    }
    assert(ok);

    std::cout << summary_score << std::endl;
    for (const auto& player : best_players) {
        std::cout << player.index << ' ';
    }

    return 0;
}