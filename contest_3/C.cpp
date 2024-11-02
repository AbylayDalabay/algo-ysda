#include <algorithm>
#include <cassert>
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

    bool operator==(const MemoryBlock& other) const {
        return size == other.size && first_index == other.first_index;
    }
};

std::ostream& operator<<(std::ostream& os, const MemoryBlock& memory_block) {
    os << "[" << memory_block.first_index << ", " << memory_block.size << ", "
       << (memory_block.is_allocated ? "alloc" : "not alloc") << "]";
    return os;
}

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
    std::cout << "Pushed to the heap: " << *memory_blocks.begin() << std::endl;

    std::vector<MemoryBlockIterator> allocations;
    allocations.assign(query_count, memory_blocks.end());

    std::cout << "Initial state" << std::endl;
    for (const MemoryBlock& memory_block : memory_blocks) {
        std::cout << memory_block << "-";
    }
    std::cout << std::endl;

    for (int query_index = 0; query_index < query_count; ++query_index) {
        std::cout << "Test case: #" << query_index + 1 << std::endl;
        int value;
        std::cin >> value;

        if (value > 0) {
            std::cout << "Starting to allocate " << value << " memory"
                      << std::endl;

            // allocate memory using heap
            MemoryBlockIterator memory_block_it = max_heap.top();
            max_heap.pop();

            std::cout << "Heap top memory: " << *memory_block_it << std::endl;

            if (memory_block_it->size >= value) {
                // allocate first blocks
                int size = memory_block_it->size;
                int first_index = memory_block_it->first_index;

                MemoryBlock new_block(value, first_index, true);
                MemoryBlockIterator new_block_it =
                    memory_blocks.insert(memory_block_it, new_block);

                assert(new_block_it != memory_blocks.end());
                allocations[query_index] = new_block_it;
                std::cout << "Equalled allocation: " << *new_block_it
                          << std::endl;

                memory_block_it->first_index = first_index + value;
                memory_block_it->size = size - value;

                if (memory_block_it->size == 0) {
                    std::cout << "Deleted because empty: " << *memory_block_it << std::endl;
                    memory_blocks.erase(memory_block_it);
                } else {
                    std::cout << "Pushed to the heap: " << *memory_block_it
                              << std::endl;
                    max_heap.push(memory_block_it);
                    std::cout << "Pushed to the heap: " << *memory_block_it
                              << std::endl;
                }
                std::cout << "allocated" << std::endl;
                std::cout << "answer: " << new_block.first_index << std::endl;
            } else {
                std::cout << "not enough memory" << std::endl;
                std::cout << "answer: " << -1 << std::endl;
            }
        } else {
            // decline allocation
            std::cout << "Free memory query" << std::endl;
            std::cout << "Allocations history: " << std::endl;
            for (int index = 0; index <= query_index; ++index) {
                std::cout << "index: " << index << ": ";
                if (allocations[index] == memory_blocks.end()) {
                    std::cout << "Hui" << std::endl;
                } else {
                    std::cout << *allocations[index] << std::endl;
                }
            }
            std::cout << std::endl;

            int allocation_index = -value;
            --allocation_index;

            MemoryBlockIterator memory_block_it = allocations[allocation_index];

            if (memory_block_it == memory_blocks.end()) {
                // assert(!memory_block_it->is_allocated);
                std::cout << "not allocated memory, unable to free"
                          << std::endl;
            } else {
                std::cout << "Starting to delete" << std::endl;
                std::cout << "Want to delete this memory: " << *memory_block_it
                          << std::endl;

                assert(memory_block_it->is_allocated);
                memory_block_it->is_allocated = false;

                std::cout << "Heap size before deletion: " << max_heap.size()
                          << std::endl;
                MaxHeapIterator<MemoryBlockIterator> max_heap_copy;
                while (!max_heap.empty()) {
                    MemoryBlockIterator top = max_heap.top();
                    max_heap.pop();
                    if (top != memory_block_it) {
                        max_heap_copy.push(top);
                        std::cout << "Pushed to the heap: " << *top
                                  << std::endl;
                    } else {
                        std::cout << "Found and deleted from heap: ";
                        std::cout << *top << std::endl;
                    }
                }
                max_heap = max_heap_copy;
                std::cout << "Heap size after deletion: " << max_heap.size()
                          << std::endl;
            }
        }
        for (const MemoryBlock& memory_block : memory_blocks) {
            std::cout << memory_block << "-";
        }
        std::cout << std::endl;
        std::cout << "___________________________________________" << std::endl;
    }
}