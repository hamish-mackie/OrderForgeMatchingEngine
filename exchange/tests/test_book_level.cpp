#include <gtest/gtest.h>

#include "book_level.h"

TEST(BookLevel, add_order) {
    const Price price;
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();
    auto bl = BookLevel(price);
    const auto order = Order(price, qty, BUY, OPEN, LIMIT, 12345, id);

    ASSERT_EQ(bl.total_quantity(), Quantity(0));
    ASSERT_EQ(bl.size(), 0);
    bl.add_order(order);
    ASSERT_EQ(bl.size(), 1);
    ASSERT_EQ(bl.total_quantity(), qty);
}

TEST(BookLevel, remove_order) {
    const Price price;
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();
    auto bl = BookLevel(price);
    const auto order = Order(price, qty, BUY, OPEN, LIMIT, 12345, id);

    bl.add_order(order);
    ASSERT_EQ(bl.size(), 1);
    ASSERT_EQ(bl.total_quantity(), qty);
    bl.remove_order(id);
    ASSERT_EQ(bl.size(), 0);
    ASSERT_EQ(bl.total_quantity(), Quantity(0));
}

TEST(BookLevel, stress_test_10000_orders_random_removal) {
    const Price price;
    const int num_orders = 10000;
    auto bl = BookLevel(price);
    std::vector<OrderId> ids;
    ids.reserve(num_orders);

    for (int i = 0; i < num_orders; ++i) {
        Quantity qty = Quantity(1);
        const OrderId id = gen_random_order_id();
        ids.push_back(id);
        bl.add_order(Order(price, qty, BUY, OPEN, LIMIT, 12345 + i, id));
    }

    ASSERT_EQ(bl.size(), num_orders);
    ASSERT_EQ(bl.total_quantity(), Quantity(num_orders));

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(ids.begin(), ids.end(), g);

    for (const auto& id : ids) {
        bl.remove_order(id);
    }

    ASSERT_EQ(bl.size(), 0);
    ASSERT_EQ(bl.total_quantity(), Quantity(0));
}