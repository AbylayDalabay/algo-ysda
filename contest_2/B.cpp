#include <iostream>
#include <random>
#include <vector>

struct Player {
    int64_t efficiency;
    int index;

    static bool CompByEfficiency(const Player& lhs, const Player& rhs) {
        return lhs.efficiency < rhs.efficiency;
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

std::vector<Player> InputPlayersVector() {
    int players_size;
    std::cin >> players_size;
    std::vector<Player> players(players_size);
    for (int current_index = 0; current_index < players_size; ++current_index) {
        std::cin >> players[current_index].efficiency;
        players[current_index].index = current_index + 1;
    }
    return players;
}

template <typename Iterator, typename Generator>
Iterator SelectPivot(Iterator first, Iterator last, Generator generator) {
    size_t distance = std::distance(first, last);

    std::uniform_int_distribution<int> distrib(0, distance - 1);

    const int kPivotIndex = distrib(generator);
    return first + kPivotIndex;
}

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

template <typename Iterator = std::vector<Player>::iterator>
std::vector<Player> BuildMostEffectiveSolidaryTeam(
    std::vector<Player> players) {
    auto is_solidary_segment = [](Iterator first, Iterator last) -> bool {
        if (std::distance(first, last) <= 1) {
            return true;
        }
        return first->efficiency + std::next(first)->efficiency >=
               last->efficiency;
    };

    QuickSort(players.begin(), players.end(), Player::CompByEfficiency);

    int64_t best_summary_effectiveness = 0;
    int64_t current_summary_effectiveness = 0;

    PlayersTeam<Iterator> best_team{players.begin(),
                                    std::next(players.begin())};

    Iterator current_first = players.begin();
    for (Iterator current_last = players.begin(); current_last != players.end();
         ++current_last) {
        current_summary_effectiveness += current_last->efficiency;

        while (current_first != current_last &&
               !is_solidary_segment(current_first, current_last)) {
            current_summary_effectiveness -= current_first->efficiency;
            ++current_first;
        }

        if (current_summary_effectiveness > best_summary_effectiveness) {
            best_summary_effectiveness = current_summary_effectiveness;
            best_team.first = current_first;
            best_team.last = std::next(current_last);
        }
    }

    return std::vector<Player>(best_team.first, best_team.last);
}

template <typename T>
T Accumulate(const std::vector<T>& vector) {
    T summary_efficiency = 0;
    for (const auto& x : vector) {
        summary_efficiency += x;
    }
    return summary_efficiency;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Player> players = InputPlayersVector();

    std::vector<Player> best_team = BuildMostEffectiveSolidaryTeam(players);

    QuickSort(best_team.begin(), best_team.end(), Player::CompByIndex);

    int64_t summary_effectiveness = 0;
    for (const auto& player : best_team) {
        summary_effectiveness += player.efficiency;
    }

    std::cout << summary_effectiveness << std::endl;
    for (const auto& player : best_team) {
        std::cout << player.index << ' ';
    }

    return 0;
}