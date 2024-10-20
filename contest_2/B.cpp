#include <algorithm>
#include <cassert>
#include <cwchar>
#include <iostream>
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
};

template <typename Iterator>
struct PlayersTeam {
    Iterator first;
    Iterator last;
};

template <typename Iterator, typename Predicate>
Iterator Partition(Iterator first, Iterator last, Predicate pred) {
    Iterator split_it = first;

    for (Iterator current_it = first; current_it != last; ++current_it) {
        if (pred(*current_it)) {
            std::swap(*split_it, *current_it);
            ++split_it;
        }
    }

    return split_it;
}

template <typename Iterator, typename Generator>
Iterator SelectPivot(Iterator first, Iterator last, Generator generator) {
    size_t distance = std::distance(first, last);

    std::uniform_int_distribution<int> distrib(0, distance - 1);

    const int kPivotIndex = distrib(generator);
    return first + kPivotIndex;
}

template <typename Iterator, typename Comparator, typename Generator>
void QuickSort(Iterator first, Iterator last, Comparator comparator,
               Generator generator) {
    if (std::distance(first, last) <= 1) {
        return;
    }

    const auto kPivot = *SelectPivot(first, last, generator);

    const Iterator kEqualIterator =
        Partition(first, last, [kPivot, comparator](const auto& elem) {
            return comparator(elem, kPivot);
        });

    const Iterator kGreaterIterator =
        Partition(kEqualIterator, last, [kPivot, comparator](const auto& elem) {
            return !comparator(kPivot, elem);
        });

    QuickSort(first, kEqualIterator, comparator);
    QuickSort(kGreaterIterator, last, comparator);
}

template <typename Iterator, typename Comparator>
void QuickSort(Iterator first, Iterator last, Comparator comparator) {
    const int kRandomState = 667;
    std::mt19937 random_generator(kRandomState);

    QuickSort(first, last, comparator, random_generator);
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

    PlayersTeam<Iterator> best_team{first, std::next(first)};

    Iterator current_first = first;
    for (Iterator current_last = first; current_last != last; ++current_last) {
        current_summary_score += current_last->score;

        while (current_first < current_last &&
               !is_solidary(current_first, current_last)) {
            current_summary_score -= current_first->score;
            ++current_first;
        }

        if (current_summary_score > best_summary_score) {
            best_summary_score = current_summary_score;
            best_team.first = current_first;
            best_team.last = std::next(current_last);
        }
    }

    return std::vector<Player>(best_team.first, best_team.last);
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

    const int kRandomState = 667;
    std::mt19937 random_generator(kRandomState);

    QuickSort(players.begin(), players.end(), Player::CompByScore);

    std::vector<Player> best_team =
        BuildMostEffectiveSolidaryTeam(players.begin(), players.end());

    int64_t summary_score = 0;
    for (const auto& player : best_team) {
        summary_score += player.score;
    }

    QuickSort(best_team.begin(), best_team.end(), Player::CompByIndex);

    std::cout << summary_score << std::endl;
    for (const auto& player : best_team) {
        std::cout << player.index << ' ';
    }

    return 0;
}