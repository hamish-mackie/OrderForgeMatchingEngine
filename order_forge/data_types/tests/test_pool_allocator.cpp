#include <gtest/gtest.h>

#include "pool_allocator.h"

using namespace of;

TEST(TestPoolAllocator, SingleAllocation) {
    PoolAllocator<int> allocator;
    int* p1 = allocator.allocate(1);
    *p1 = 777;
    ASSERT_NE(p1, nullptr);
    ASSERT_EQ(*p1, 777);
    allocator.deallocate(p1, 1);
}

TEST(TestPoolAllocator, MultipleAllocations) {
    PoolAllocator<int> allocator;
    int* p1 = allocator.allocate(5);
    ASSERT_NE(p1, nullptr);
    allocator.deallocate(p1, 5);

    int* p2 = allocator.allocate(10);
    ASSERT_NE(p2, nullptr);
    allocator.deallocate(p2, 10);
}

TEST(TestPoolAllocator, ObjectConstruction) {
    PoolAllocator<std::pair<int, double>> allocator;
    using ValueType = std::pair<int, double>;

    ValueType* p1 = allocator.allocate(1);
    allocator.construct(p1, 1, 3.14);

    ASSERT_EQ(p1->first, 1);
    ASSERT_EQ(p1->second, 3.14);

    allocator.destroy(p1);
    allocator.deallocate(p1, 1);
}

// TEST(TestPoolAllocator, UnorderedMapWithAllocator) {
//     using Key = int;
//     using Value = int;
//     using Allocator = PoolAllocator<std::pair<const Key, Value>>;
//     using Hash = std::hash<Key>;
//     using Pred = std::equal_to<Key>;
//
//     auto map = std::unordered_map<Key, Value, Hash, Pred, Allocator>(0, Hash(), Pred(), Allocator());
//
//     map[1] = 10;
//     map[2] = 20;
//     map[3] = 30;
//
//     ASSERT_EQ(map[1], 10);
//     ASSERT_EQ(map[2], 20);
//     ASSERT_EQ(map[3], 30);
// }

TEST(TestPoolAllocator, LargeAllocation) {
    PoolAllocator<int> allocator;
    int* p1 = allocator.allocate(1024);
    ASSERT_NE(p1, nullptr);
    allocator.deallocate(p1, 1024);
}

TEST(TestPoolAllocator, AllocateAndDeallocate) {
    PoolAllocator<int> allocator;

    std::vector<int*> pointers;
    for (int i = 0; i < 100; ++i) {
        int* p = allocator.allocate(1);
        *p = i;
        pointers.push_back(p);
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(*pointers[i], i);
        allocator.deallocate(pointers[i], 1);
    }
}

TEST(TestPoolAllocator, PoolExpansion) {
    PoolAllocator<int> allocator(2);
    int* p1 = allocator.allocate(2);
    int* p2 = allocator.allocate(2);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    allocator.deallocate(p1, 2);
    allocator.deallocate(p2, 2);
}
