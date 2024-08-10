#include <gtest/gtest.h>

#include "logger.h"

class TestLogger : testing::Test {

};

TEST(TestLogger, test) {
    Logger::get_instance();
}