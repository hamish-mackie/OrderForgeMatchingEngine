#pragma once

#include <memory>

#include "singleton_wrapper.h"

namespace of {


template<typename T>
class PoolAllocator {
public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit PoolAllocator() : item_count_(0) {}
    explicit PoolAllocator(size_type initial_count) : item_count_(initial_count) { expand_pool(initial_count); }

    template<typename U>
    explicit PoolAllocator(const PoolAllocator<U>&) noexcept : PoolAllocator(1024) {}

    pointer allocate(size_type n) {
        if (n != 1) {
            throw std::bad_alloc();
        }

        if (free_list_.empty()) {
            expand_pool(n);
        }

        pointer result = free_list_.back();
        free_list_.pop_back();
        return result;
    }

    void deallocate(pointer ptr, size_type = 1) { free_list_.push_back(ptr); }

    template<typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };

    template<typename U, typename... Args>
    void construct(U *p, Args &&...args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) {
        p->~U();
    }

    ~PoolAllocator() {
        for (auto* block: pool_blocks_) {
            ::operator delete[](block);
        }
    }

    void expand_pool(size_type n) {
        size_t new_pool_size = std::max(item_count_, n);

        T* ptr = static_cast<pointer>(::operator new[](sizeof(T) * new_pool_size));
        pool_blocks_.push_back(ptr);
        for (size_t i = 0; i < new_pool_size; ++i) {
            free_list_.push_back(&pool_blocks_.back()[i]);
        }
        item_count_ = new_pool_size * 2;
    }

    template<typename U>
    bool operator==(const PoolAllocator<U>&) const noexcept {
        return true;
    }

    template<typename U>
    bool operator!=(const PoolAllocator<U>& other) const noexcept {
        return !(*this == other);
    }

private:
    size_type item_count_;
    std::vector<pointer> pool_blocks_;
    std::vector<pointer> free_list_;
};

} // namespace of
