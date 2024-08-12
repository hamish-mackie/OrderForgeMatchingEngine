#include <gtest/gtest.h>

#include "ring_buffer.h"

TEST(RingBufferTest, BasicWriteRead) {
    RingBuffer buffer(1024, 2); // 2 blocks of 1KB each

    char* write_ptr = buffer.get_write_pointer(100);
    ASSERT_NE(write_ptr, nullptr);
    strcpy(write_ptr, "Hello World");

    buffer.forward_write_pointer(100);

    ASSERT_TRUE(buffer.can_read());

    char* read_ptr = buffer.get_read_pointer();
    ASSERT_NE(read_ptr, nullptr);
    ASSERT_STREQ(read_ptr, "Hello World");

    buffer.forward_read_pointer(100);

    ASSERT_FALSE(buffer.can_read());
}

TEST(RingBufferTest, WrapAroundWriteRead) {
    RingBuffer buffer(1024, 2); // 2 blocks of 1KB each

    char* write_ptr = buffer.get_write_pointer(1024);
    ASSERT_NE(write_ptr, nullptr);
    memset(write_ptr, 'A', 1024);

    buffer.forward_write_pointer(1024);

    write_ptr = buffer.get_write_pointer(512);
    ASSERT_NE(write_ptr, nullptr);
    memset(write_ptr, 'B', 512);

    buffer.forward_write_pointer(512);

    char* read_ptr = buffer.get_read_pointer();
    ASSERT_NE(read_ptr, nullptr);
    for (int i = 0; i < 1024; ++i) {
        ASSERT_EQ(read_ptr[i], 'A');
    }
    buffer.forward_read_pointer(1024);

    read_ptr = buffer.get_read_pointer();
    ASSERT_NE(read_ptr, nullptr);
    for (int i = 0; i < 512; ++i) {
        ASSERT_EQ(read_ptr[i], 'B');
    }
    buffer.forward_read_pointer(512);

    ASSERT_FALSE(buffer.can_read());
}