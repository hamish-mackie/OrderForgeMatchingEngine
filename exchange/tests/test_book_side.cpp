#include <gtest/gtest.h>

#include "book_side.h"

TEST(BookSide, Construct) {
    BookSide bs = BookSide(Side::BUY, Price(100), Price(0.01));
    ASSERT_EQ(bs.start_price().descaled_value(), 95);
    bs.end_price();

}