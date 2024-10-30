#include <cstdint>
#include <iostream>
#include <numeric>
#include <ostream>
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

std::vector<Player> InputPlayersVector(std::istream& in = std::cin) {
    int players_size;
    in >> players_size;
    std::vector<Player> players;
    players.reserve(players_size);
    for (int current_index = 0; current_index < players_size; ++current_index) {
        int64_t current_efficiency;
        in >> current_efficiency;
        players.emplace_back(current_efficiency, current_index + 1);
    }
    return players;
}

template <typename Iterator>
struct PlayersTeam {
    Iterator first;
    Iterator last;
};

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
    static std::mt19937 random_generator(kRandomState);

    QuickSort(first, last, comparator, random_generator);
}

std::vector<Player> BuildMostEffectiveSolidaryTeam(
    std::vector<Player> players) {
    using Iterator = std::vector<Player>::iterator;

    QuickSort(players.begin(), players.end(), Player::CompByEfficiency);

    int64_t best_summary_effectiveness = 0;
    int64_t current_summary_effectiveness = 0;

    PlayersTeam<Iterator> best_team{players.begin(),
                                    std::next(players.begin())};

    PlayersTeam<Iterator> current_team;

    current_team.first = players.begin();

    auto is_solidary_segment = [](Iterator first, Iterator last) -> bool {
        if (std::distance(first, last) <= 1) {
            return true;
        }
        return first->efficiency + std::next(first)->efficiency >=
               last->efficiency;
    };

    for (current_team.last = players.begin();
         current_team.last != players.end(); ++current_team.last) {
        current_summary_effectiveness += current_team.last->efficiency;

        while (current_team.first != current_team.last &&
               !is_solidary_segment(current_team.first, current_team.last)) {
            current_summary_effectiveness -= current_team.first->efficiency;
            ++current_team.first;
        }

        if (current_summary_effectiveness > best_summary_effectiveness) {
            best_summary_effectiveness = current_summary_effectiveness;
            best_team.first = current_team.first;
            best_team.last = std::next(current_team.last);
        }
    }

    return {best_team.first, best_team.last};
}

int64_t SummaryEfficiency(const std::vector<Player>& players) {
    int64_t summary_efficiency =
        std::accumulate(players.begin(), players.end(), static_cast<int64_t>(0),
                        [](int64_t sum, const Player& player) {
                            return sum + player.efficiency;
                        });

    return summary_efficiency;
}

std::ostream& OutputVector(const std::vector<Player>& players,
                           std::ostream& os = std::cout) {
    os << SummaryEfficiency(players) << std::endl;
    for (const Player& player : players) {
        os << player.index << ' ';
    }
    return os;
}

int main() {
    auto players = InputPlayersVector();

    auto best_team = BuildMostEffectiveSolidaryTeam(players);

    QuickSort(best_team.begin(), best_team.end(), Player::CompByIndex);

    OutputVector(best_team);

    return 0;
}