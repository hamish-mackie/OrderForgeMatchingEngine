#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../logger/logger.h"

template<typename T>
class PoolAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit PoolAllocator(size_t initial_size = 1024) : pool_size_(initial_size) {
        expand_pool(initial_size);
    }

    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) noexcept : PoolAllocator(1024) {}

    pointer allocate(size_type n) {
        if (free_list_.size() < n) {
            expand_pool(n);
        }
        pointer result = free_list_.back();
        free_list_.resize(free_list_.size() - n);
        return result;
    }

    void deallocate(pointer ptr, size_type) {
        free_list_.push_back(ptr);
    }

    template<typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };

    template<typename U, typename ... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    ~PoolAllocator() {
        for (auto* block : pool_blocks_) {

            free(block);
        }
    }

private:
    size_t pool_size_;
    std::vector<pointer> pool_blocks_;
    std::vector<pointer> free_list_;

    void expand_pool(size_type n) {
        size_t new_pool_size = std::max(pool_size_, n);

        T* ptr = static_cast<pointer>(malloc(new_pool_size * sizeof(T)));
        pool_blocks_.push_back(ptr);
        for (size_t i = 0; i < new_pool_size; ++i) {
            free_list_.push_back(&pool_blocks_.back()[i]);
        }
        pool_size_ = new_pool_size * 2;
    }
};

template<typename T>
class SingleTonWrapper {
    using Type = T;
public:
    static Type& get_instance() {
        static Type instance;
        return instance;
    }
    SingleTonWrapper(const SingleTonWrapper&) = delete;
    SingleTonWrapper& operator=(const SingleTonWrapper&) = delete;
private:
    SingleTonWrapper() = default;
};