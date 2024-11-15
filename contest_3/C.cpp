#include <cassert>
#include <iostream>
#include <list>
#include <queue>
#include <stdexcept>
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
struct MemoryBlockPointer {
    Iterator it;
    bool exist;

    MemoryBlockPointer(Iterator it, bool exist) : it(it), exist(exist) {}
};

template <typename Pointer>
struct CompareMemoryBlockPointers {
    bool operator()(Pointer lhs, Pointer rhs) { return *(lhs.it) < *(rhs.it); }
};

template <typename Pointer>
class MaxHeapPointer {
private:
    std::priority_queue<Pointer, std::vector<Pointer>,
                        CompareMemoryBlockPointers<Pointer>>
        pointers_heap_;

public:
    MaxHeapPointer() {}

    void Push(Pointer pointer) { pointers_heap_.push(pointer); }
    Pointer Top() {
        while (!pointers_heap_.empty()) {
            Pointer top_pointer = pointers_heap_.top();
            pointers_heap_.pop();
            if (top_pointer.exist) {
                return top_pointer;
            }
        }
        throw std::runtime_error("MaxHeapPointer empty");
    }
};

int main() {
    int memory_size;
    int query_count;
    std::cin >> memory_size >> query_count;

    std::list<MemoryBlock> memory_blocks;
    memory_blocks.push_back({memory_size, 1, false});

    using MemoryBlockIterator = std::list<MemoryBlock>::iterator;
    using MemoryBlockPointer = MemoryBlockPointer<MemoryBlockIterator>;

    MaxHeapPointer<MemoryBlockPointer> max_heap;
    max_heap.Push({memory_blocks.begin(), true});

    std::cout << *max_heap.Top().it << std::endl;
}