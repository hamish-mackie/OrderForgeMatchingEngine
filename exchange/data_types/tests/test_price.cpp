#include <gtest/gtest.h>
#include "price.h"

TEST(PriceTest, Scaling) {
    const ScaledPrice<100> price1(123);
    EXPECT_EQ(price1.price(), 12300);
    EXPECT_DOUBLE_EQ(price1.descaled_price(), 123.0);

    const ScaledPrice<1000> price2(123);
    EXPECT_EQ(price2.price(), 123000);
    EXPECT_DOUBLE_EQ(price2.descaled_price(), 123.0);
}


