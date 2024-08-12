#include <magic_enum.hpp>
#include <gtest/gtest.h>

#include "logger.h"

constexpr std::string_view test_prepend = "test_prepend";

#define LOG_TEST(test_struct) Logger::get_instance().write_buffer<TestStructOriginal, TestStructLog>(LogType::TEST_STRUCT, LOG_PREPEND("TEST_STRUCT"), test_struct);

TEST(TestLogger, test) {
    auto& l = Logger::get_instance(true, 1024);
    auto test_struct = TestStructOriginal(1, 2, 3);
    REGISTER_TYPE(TEST_STRUCT, TestStruct);
    LOG_TEST(test_struct);
    LOG_TEST(test_struct);
    sleep(1);
    Logger::get_instance().stop();
}

TEST(TestLogger, test_alot) {
    auto& l = Logger::get_instance(true, 1024);
    auto test_struct = TestStructOriginal(111, 222, 333);
    REGISTER_TYPE(TEST_STRUCT, TestStruct);
    for(int i = 0; i < 10; ++i) {
        LOG_TEST(test_struct);
    }
    sleep(1);
    Logger::get_instance().stop();
}