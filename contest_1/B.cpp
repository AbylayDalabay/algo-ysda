#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

int main() {
    std::string bracket_sequence;
    std::cin >> bracket_sequence;

    std::stack<char> prefix_compress;

    std::vector<std::pair<char, char>> pairwise_brackets = {
        {'(', ')'}, {'[', ']'}, {'{', '}'}};

    size_t last_prefix_length = 0;
    for (const auto& bracket : bracket_sequence) {
        bool erased_bracket = false;

        for (const auto& bracket_pair : pairwise_brackets) {
            const char opener_bracket = bracket_pair.first;
            const char closer_bracket = bracket_pair.second;

            if (bracket == closer_bracket) {
                if (!prefix_compress.empty() &&
                    prefix_compress.top() == opener_bracket) {
                    prefix_compress.pop();
                    erased_bracket = true;
                } else {
                    std::cout << last_prefix_length << std::endl;
                    return 0;
                }
            }
        }
        if (!erased_bracket) {
            prefix_compress.push(bracket);
        }
        ++last_prefix_length;
    }

    if (prefix_compress.empty()) {
        std::cout << "CORRECT" << std::endl;
    } else {
        std::cout << last_prefix_length << std::endl;
    }

    return 0;
}