#include <ios>
#include <iostream>
#include <vector>

#include "fixed_set_review.h"

int main() {
    std::vector<int> ar = {0, 4, 5};

    FixedSet set;
    set.Initialize(ar);

    std::cout << std::boolalpha << set.Contains(0) << std::endl;
}