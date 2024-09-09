#include <gtest/gtest.h>

#include "chunked_allocator.h"

using namespace of;

class TestChunk : public ::testing::Test {
protected:
    Chunk<int> allocator;
};

// Test allocation functionality
TEST_F(TestChunk, AllocateSuccessfully) {
    auto* ptr = allocator.allocate();
    *ptr = 0;
    ASSERT_TRUE(ptr);
    EXPECT_EQ(*ptr, 0);
}

TEST_F(TestChunk, AllocatorBecomesFull) {
    for (size_t i = 0; i < of::DEFAULT_CHUNK_SIZE; ++i) {
        allocator.allocate();
    }
    EXPECT_TRUE(allocator.is_full());
    auto* ptr = allocator.allocate();
    EXPECT_FALSE(ptr);
}

TEST_F(TestChunk, DeallocateSuccessfully) {
    allocator.allocate();
    auto* ptr = allocator.allocate();
    allocator.allocate();
    allocator.allocate();
    ASSERT_TRUE(ptr);
    allocator.deallocate(ptr);
    EXPECT_FALSE(allocator.completely_used());
}

TEST_F(TestChunk, CompletelyUsedAfterDeallocations) {
    std::vector<int*> vec;
    vec.reserve(64);

    for (size_t i = 0; i < DEFAULT_CHUNK_SIZE; ++i) {
        vec.push_back(allocator.allocate());
    }
    EXPECT_TRUE(allocator.is_full());

    for (auto* ptr: vec) {
        allocator.deallocate(ptr);
    }
    EXPECT_TRUE(allocator.completely_used());
}

class TestChunkAllocator : public ::testing::Test {
protected:
    ChunkAllocator<int>::pool_allocator* pool_allocator = nullptr;
    ChunkAllocator<int>* allocator = nullptr;

    void SetUp() override {
        pool_allocator = new ChunkAllocator<int>::pool_allocator();
        allocator = new ChunkAllocator(*pool_allocator);
    }
};

// Test allocation functionality
TEST_F(TestChunkAllocator, AllocateSuccessfully) {
    auto* ptr = allocator->allocate();
    *ptr = 5;

    ASSERT_TRUE(ptr);
    EXPECT_EQ(*ptr, 5);

    allocator->deallocate(ptr);
}
