#include <gtest/gtest.h>

#include "order_forge_types.h"

TEST(TestIdGenerator, test_macros) {
    auto order_id = get_id<Order>();
    auto trade_id = get_id<Trade>();
    ASSERT_EQ(order_id, trade_id);
    order_id = get_id<Order>();
    ASSERT_NE(order_id, trade_id);
}

TEST(TestIdGenerator, test_different_ids) {
    auto first_order_id = get_id<Order>();
    auto second_order_id = get_id<Order>();

    ASSERT_NE(first_order_id, second_order_id);
}