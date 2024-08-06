#include <gtest/gtest.h>

#include "book_side.h"

TEST(BookSide, Construct) {
    auto bs = BookSide(Side::BUY, Price(100), Price(0.01));
    ASSERT_EQ(bs.start_price().descaled_value(), 95);
    bs.end_price();

    auto bs2 = BookSide(Side::SELL, Price(100), Price(0.01));
    ASSERT_EQ(bs2.start_price().descaled_value(), 105);
    bs.end_price();

}