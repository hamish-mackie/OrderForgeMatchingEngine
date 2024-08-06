#include <gtest/gtest.h>
#include "quantity.h"


TEST(QuantityTest, DefaultConstructor) {
    const Quantity qty;
    EXPECT_EQ(qty.quantity(), 0);
    EXPECT_DOUBLE_EQ(qty.descaled_quantity(), 0.0);
}

TEST(QuantityTest, Constructor) {
    const Quantity qty(1.23);
    EXPECT_EQ(qty.quantity(), 123000000);
    EXPECT_DOUBLE_EQ(qty.descaled_quantity(), 1.23);
}

TEST(QuantityTest, Equality) {
    const Quantity qty1(1.23);
    const Quantity qty2(1.23);
    const Quantity qty3(2.46);
    EXPECT_TRUE(qty1 == qty2);
    EXPECT_FALSE(qty1 == qty3);
}

TEST(QuantityTest, Addition) {
    Quantity qty1(1.23);
    const Quantity qty2(2.46);
    qty1 += qty2;
    EXPECT_EQ(qty1.quantity(), 369000000);
    EXPECT_DOUBLE_EQ(qty1.descaled_quantity(), 3.69);
}

TEST(QuantityTest, Subtraction) {
    Quantity qty1(2.46);
    const Quantity qty2(1.23);
    qty1 -= qty2;
    EXPECT_EQ(qty1.quantity(), 123000000);
    EXPECT_DOUBLE_EQ(qty1.descaled_quantity(), 1.23);
}

TEST(QuantityTest, OutputStream) {
    const Quantity qty(1.23);
    std::stringstream ss;
    ss << qty;
    EXPECT_EQ(ss.str(), "1.23");
}


