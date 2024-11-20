#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

class Block {
private:
    int size_;
    int pref_size_;
    std::vector<int> array_;

public:
    Block() : size_(0), pref_size_(0) {}

    int Size() const { return size_; }

    
};

class SqrtDecomposeSet {
private:
    static const int kBlockSize = 3;
    static const int kMaxBlockCount = 500;

    struct Value {
        int value;
        int index;
    };

    int block_count_ = 0;

    std::vector<std::vector<Value>> blocks_;

    void Build(std::vector<int> array) {
        sort(array.begin(), array.end());

        blocks_.assign(kMaxBlockCount, {});
        auto current_block = blocks_.begin();

        block_count_ = 1;

        int current_index = 0;
        for (auto x : array) {
            if (current_block->size() == kBlockSize) {
                ++current_block;
                ++block_count_;
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

            if (pref_sizes + current_block_size > index) {
                return BlockState{block_index, index - pref_sizes};
            }
            pref_sizes += current_block_size;
        }
        return BlockState{block_count_, 0};
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
        ++block_count_;
    }

    void Insert(int index, int value) {
        BlockState block_state = FindBlockState(index);

        std::cout << "Insert index: " << index << std::endl;
        std::cout << "Found block_state: " << block_state.block_index << ", "
                  << block_state.index_inside_block << std::endl;

        if (GetBlockSize(block_state.block_index) == 2 * kBlockSize) {
            // Reconstruct ...
            Reconstruct(block_state.block_index);
            std::cout << "Reconstructed: " << std::endl;
        }

        block_state = FindBlockState(index);
        if (!(GetBlockSize(block_state.block_index) < 2 * kBlockSize)) {
            std::cout << "block state assert: "
                      << GetBlockSize(block_state.block_index) << std::endl;

            assert(false);
        }

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

    void Erase(int index) {
        BlockState block_state = FindBlockState(index);

        std::vector<Value>& current_block = blocks_[block_state.block_index];
        std::vector<Value>::iterator delete_it =
            current_block.begin() + block_state.index_inside_block;

        current_block.erase(delete_it);
    }

    static int CountNotGreaterInBlock(const std::vector<Value>& block,
                                      int upper_bound) {
        int left_index = 0;
        int right_index = block.size();

        while (left_index + 1 < right_index) {
            int mid_index = (left_index + right_index) / 2;

            if (block[mid_index].value <= upper_bound) {
                left_index = mid_index;
            } else {
                right_index = mid_index;
            }
        }

        return left_index;
    }

    static int CountNotGreaterInBlockCheckIndices(
        const std::vector<Value>& block, int left_index, int right_index,
        int upper_bound) {
        int answer = 0;
        for (const Value& value : block) {
            if (value.value <= upper_bound && left_index <= value.index &&
                value.index <= right_index) {
                ++answer;
            }
        }

        return answer;
    }

    int CountNotGreater(int left_index, int right_index, int upper_bound) {
        if (left_index > right_index) {
            return 0;
        }

        int answer = 0;

        BlockState left_block_state = FindBlockState(left_index);
        BlockState right_block_state = FindBlockState(right_index);

        for (int current_block_index = left_block_state.block_index + 1;
             current_block_index < right_block_state.block_index;
             ++current_block_index) {
            answer += CountNotGreaterInBlock(blocks_[current_block_index],
                                             upper_bound);
        }

        answer += CountNotGreaterInBlockCheckIndices(
            blocks_[left_block_state.block_index], left_index, right_index,
            upper_bound);

        if (left_block_state.block_index != right_block_state.block_index) {
            answer += CountNotGreaterInBlockCheckIndices(
                blocks_[right_block_state.block_index], left_index, right_index,
                upper_bound);
        }

        return answer;
    }
};

int main() {
    // std::ios_base::sync_with_stdio(false);
    // std::cin.tie(nullptr);
    // std::cout.tie(nullptr);

    int array_size;
    std::cin >> array_size;

    std::vector<int> array(array_size);
    for (auto& x : array) {
        std::cin >> x;
    }

    SqrtDecomposeSet set(array);

    set.DisplayBlocks();

    std::cout << "Queries" << std::endl;

    char current_operation;
    while (std::cin >> current_operation) {
        if (current_operation == '-') {
            std::cout << "Erase operation: " << std::endl;
            int index;
            std::cin >> index;
            set.Erase(index);
        } else if (current_operation == '+') {
            std::cout << "Insert operation: " << std::endl;
            int index;
            int value;
            std::cin >> index >> value;
            set.Insert(index, value);
        } else if (current_operation == '?') {
            std::cout << "Count operation: " << std::endl;
            int left_index;
            int right_index;
            int upper_bound;

            std::cin >> left_index >> right_index;
            std::cin >> upper_bound;

            int answer =
                set.CountNotGreater(left_index, right_index, upper_bound);

            std::cout << "count query answer: " << answer << std::endl;
        }
        set.DisplayBlocks();
    }

    //

    // const int kOtak = 100;

    // set.Insert(0, kOtak);
    // set.Insert(0, kOtak + 1);
    // set.Insert(0, kOtak + 2);

    // set.DisplayBlocks();

    // set.Insert(0, kOtak + 3);

    // set.DisplayBlocks();

    // set.Erase(5);

    // set.DisplayBlocks();
}