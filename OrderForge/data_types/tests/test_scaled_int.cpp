#include <unordered_set>
#include <gtest/gtest.h>
#include "scaled_int.h"

using int_type = ScaledInt<int64_t, 100>;

TEST(ScaledIntTest, Scaling) {
    const int_type price1(123);
    EXPECT_EQ(price1.value(), 12300);
    EXPECT_DOUBLE_EQ(price1.descaled_value(), 123.0);

    const ScaledInt<int64_t, 1000> price2(123);
    EXPECT_EQ(price2.value(), 123000);
    EXPECT_DOUBLE_EQ(price2.descaled_value(), 123.0);
}

TEST(ScaledIntTest, Addition) {
    int_type a(10.0);
    int_type b(5.0);
    int_type result = a + b;
    EXPECT_EQ(result.descaled_value(), 15.0);
}

TEST(ScaledIntTest, Subtraction) {
    int_type a(10.0);
    int_type b(5.0);
    int_type result = a - b;
    EXPECT_EQ(result, int_type(5.0));
}

TEST(ScaledIntTest, Multiplication) {
    int_type a(10.0);
    int_type b(5.0);
    ScaledInt<int64_t,100> result = a * b;
    EXPECT_EQ(result, int_type(50.0));
}

TEST(ScaledIntTest, Division) {
    int_type a(10.0);
    int_type b(5.0);
    int_type result = a / b;
    EXPECT_EQ(result, int_type(2.0));
}

TEST(ScaledIntTest, Hashing) {
    int_type a(55);
    int_type b(64);
    std::unordered_set<int_type> s;
    s.emplace(a);
    s.emplace(b);
    ASSERT_TRUE(s.contains(a));
    s.erase(a);
    ASSERT_EQ(s.size(), 1);
}
