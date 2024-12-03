#include <gtest/gtest.h>

#include "book_side.h"

using namespace of;

TEST(TestBookSide, Construct) {
    auto ts = TickSize(0.01);
    auto symbol = "test";
    auto bs = BookSide<std::greater<>>(symbol, BUY, ts);
    auto bs2 = BookSide<std::greater<>>(symbol, SELL, ts);
}
