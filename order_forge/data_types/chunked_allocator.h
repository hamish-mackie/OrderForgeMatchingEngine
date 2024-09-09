#pragma once

#include <bitset>
#include <pool_allocator.h>

// Chunk
//      Manages each allocated chunk
//      Holds an array of objects
//      Uses bitmask to check which ones can be set

// Chunk Allocator
//      Contains classes and a reference to pool allocator
//      Requests new chunks from pool allocator

namespace of {

constexpr std::size_t DEFAULT_CHUNK_SIZE = 128;

template<typename T, std::size_t chunk_size = DEFAULT_CHUNK_SIZE>
class Chunk {
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using size_type = std::size_t;

    using bitset = std::bitset<chunk_size>;
    using chunk_array = std::array<value_type, chunk_size>;

    bool is_full() { return index_ >= chunk_size; }
    bool completely_used() { return bitset_.none(); }

    pointer allocate() {
        if (is_full()) {
            return nullptr;
        }
        bitset_.set(index_++);
        return &chunk_[index_ - 1];
    }

    bool deallocate(pointer ptr) {
        if (!is_valid(ptr)) {
            return false;
        }
        bitset_.reset(std::distance(chunk_.begin(), ptr));
        return true;
    }

    bool is_valid(pointer ptr) { return ptr >= chunk_.begin() && ptr < chunk_.end(); }

private:
    uint32_t index_{0};
    bitset bitset_;
    chunk_array chunk_;
};

template<typename T, std::size_t chunk_size = DEFAULT_CHUNK_SIZE>
class ChunkAllocator {
public:
    using value_type = T;
    using pointer = T*;

    using chunk_type = Chunk<T, chunk_size>;
    using chunk_pointer = Chunk<T, chunk_size>*;

    using pool_allocator = PoolAllocator<chunk_type>;

    explicit ChunkAllocator(pool_allocator& pool_alloc) : pool_allocator_(pool_alloc) {
        chunks_.emplace_back(pool_allocator_.allocate(1));
    }

    pointer allocate() {
        if (!chunks_.back()->is_full()) {
            return chunks_.back()->allocate();
        } else {
            chunks_.emplace_back(pool_allocator_.allocate(1));
            return chunks_.back()->allocate();
        }
    }

    bool deallocate(pointer ptr) {
        for (auto& chunk: chunks_) {
            if (chunk->deallocate(ptr)) {
                return true;
            }
        }
        return false;
    }


private:
    pool_allocator& pool_allocator_;
    std::vector<chunk_pointer> chunks_;
};

} // namespace of
