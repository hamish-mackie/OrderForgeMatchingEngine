#include <gtest/gtest.h>

#include "book_side.h"

using namespace of;

TEST(TestBookSide, Construct) {
    auto ts = TickSize(0.01);
    auto bs = BookSide<std::greater<>>(BUY, ts);
    auto bs2 = BookSide<std::greater<>>(SELL, ts);
}
