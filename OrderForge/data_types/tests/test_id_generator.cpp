#include <gtest/gtest.h>

#include "order_forge_types.h"

TEST(TestIdGenerator, test_macros) {
    auto order_id = GET_ORDER_ID;
    auto trade_id = GET_TRADE_ID;
    ASSERT_EQ(order_id, trade_id);
    order_id = GET_ORDER_ID;
    ASSERT_NE(order_id, trade_id);
}

TEST(TestIdGenerator, test_different_ids) {
    auto first_order_id = GET_ORDER_ID;
    auto second_order_id = GET_ORDER_ID;

    ASSERT_NE(first_order_id, second_order_id);
}