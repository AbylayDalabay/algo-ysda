#include <iostream>
#include <list>
#include <queue>
#include <vector>

struct MemoryBlock {
    int size;
    int first_index;
    bool is_allocated;

    MemoryBlock(int size, int first_index, bool is_allocated)
        : size(size), first_index(first_index), is_allocated(is_allocated) {}

    bool operator<(const MemoryBlock& other) const {
        if (size != other.size) {
            return size < other.size;
        }
        return first_index > other.first_index;
    }
};

template <typename Iterator>
struct CompareIterators {
    bool operator()(Iterator lhs, Iterator rhs) { return *lhs < *rhs; }
};

template <typename Iterator>
using MaxHeapIterator = std::priority_queue<Iterator, std::vector<Iterator>,
                                            CompareIterators<Iterator>>;

int main() {
    int memory_size;
    int query_count;
    std::cin >> memory_size >> query_count;

    std::list<MemoryBlock> memory_blocks;
    memory_blocks.push_back({memory_size, 1, false});

    using MemoryBlockIterator = std::list<MemoryBlock>::iterator;

    MaxHeapIterator<MemoryBlockIterator> max_heap;
    max_heap.push(memory_blocks.begin());

    for (int query_index = 0; query_index < query_count; ++query_index) {
        int value;
        std::cin >> value;

        if (value > 0) {
            std::cout << "Trying to allocate " << value << " memory"
                      << std::endl;
            // allocate memory using heap
            MemoryBlockIterator memory_block_it = max_heap.top();
            max_heap.pop();

            if (memory_block_it->size >= value) {
                // allocate first blocks
                int size = memory_block_it->size;
                int first_index = memory_block_it->first_index;

                MemoryBlock new_block(value, first_index, true);
                MemoryBlockIterator new_block_it =
                    memory_blocks.insert(memory_block_it, new_block);
                max_heap.push(new_block_it);

                memory_block_it->first_index = first_index + value;
                memory_block_it->size = size - value;

                if (memory_block_it->size == 0) {
                    memory_blocks.erase(memory_block_it);
                } else {
                    max_heap.push(memory_block_it);
                }
                std::cout << "succesfully allocated" << std::endl;
            } else {
                std::cout << "not enough memory" << std::endl;
            }
        } else {
            // decline allocation
            std::cout << "declined allocation" << std::endl;
        }
        std::cout << query_index << " query_index list state" << std::endl;
        for (const MemoryBlock& memory_block : memory_blocks) {
            std::cout << "[" << memory_block.first_index << ", "
                      << memory_block.size << ", "
                      << (memory_block.is_allocated ? "alloc" : "not alloc")
                      << "]-";
        }
        std::cout << std::endl << std::endl;
    }
}