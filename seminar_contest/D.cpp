#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

class SqrtDecomposeSet {
private:
    static const int kBlockSize = 3;
    static const int kMaxBlockCount = 500;

    struct Value {
        int value;
        int index;
    };

    std::vector<std::vector<Value>> blocks_;

    void Build(std::vector<int> array) {
        sort(array.begin(), array.end());

        blocks_.assign(kMaxBlockCount, {});
        auto current_block = blocks_.begin();

        int current_index = 0;
        for (auto x : array) {
            if (current_block->size() == kBlockSize) {
                ++current_block;
            }
            current_block->push_back(Value{x, current_index});
            ++current_index;
        }
    }

    struct BlockState {
        int block_index;
        int index_inside_block;
    };

    int GetBlockSize(int index) {
        return static_cast<int>(blocks_[index].size());
    }

public:
    SqrtDecomposeSet(const std::vector<int>& array) { Build(array); }

    void DisplayBlocks() const {
        std::cout << "-------Blocks---------" << std::endl;
        for (const auto& block : blocks_) {
            if (block.empty()) {
                break;
            }
            std::cout << "block: ";
            for (auto x : block) {
                std::cout << "{" << x.value << ", " << x.index << "}" << ' ';
            }
            std::cout << std::endl;
        }
    }

    BlockState FindBlockState(int index) {
        int pref_sizes = 0;
        for (int block_index = 0; block_index < kMaxBlockCount; ++block_index) {
            int current_block_size = GetBlockSize(block_index);

            if (current_block_size == 0) {
                break;
            }
            if (pref_sizes + current_block_size > index) {
                return BlockState{block_index, index - pref_sizes};
            }
            pref_sizes += current_block_size;
        }
        return BlockState{-1, -1};
    }

    void Reconstruct(int target_block_index) {
        std::cout << "Reconstruct started" << std::endl;

        // target_block_index + 2, ..., kMaxBlockCount
        for (int current_block_index = kMaxBlockCount - 1;
             current_block_index >= target_block_index + 2;
             --current_block_index) {
            blocks_[current_block_index] =
                std::move(blocks_[current_block_index - 1]);
        }

        // target_block_index, target_block_index + 1
        std::vector<Value> new_block;
        std::vector<Value>& current_block = blocks_[target_block_index];

        while (current_block.size() > kBlockSize) {
            new_block.push_back(current_block.back());
            current_block.pop_back();
        }

        std::reverse(new_block.begin(), new_block.end());

        blocks_[target_block_index + 1] = new_block;
    }

    void Insert(int index, int value) {
        BlockState block_state = FindBlockState(index);

        if (GetBlockSize(block_state.block_index) == 2 * kBlockSize) {
            // Reconstruct ...
            Reconstruct(block_state.block_index);
        }

        block_state = FindBlockState(index);
        assert(GetBlockSize(block_state.block_index) < 2 * kBlockSize);

        auto& current_block = blocks_[block_state.block_index];
        auto insert_it = current_block.end();

        for (auto current_it = current_block.begin();
             current_it != current_block.end(); ++current_it) {
            if (value < current_it->value) {
                insert_it = current_it;
                break;
            }
        }

        current_block.insert(insert_it, Value{value, index});
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    SqrtDecomposeSet set(array);

    set.DisplayBlocks();

    const int kOtak = 100;

    set.Insert(0, kOtak);
    set.Insert(0, kOtak + 1);
    set.Insert(0, kOtak + 2);

    set.DisplayBlocks();

    set.Insert(0, kOtak + 3);

    set.DisplayBlocks();
}