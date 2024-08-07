#include <gtest/gtest.h>

#include "trade_producer.h"

TEST(TradeProducer, test_matching_same_size) {
    auto limit = Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(Price(99), Quantity(1), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto trade_producer = TradeProducer(market);
    trade_producer.match_order(limit);

    ASSERT_EQ(trade_producer.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(limit.status(), FILLED);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), FILLED);
}

TEST(TradeProducer, test_matching_limit_greater) {
    auto limit = Order(Price(100), Quantity(2), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(Price(99), Quantity(1), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto trade_producer = TradeProducer(market);
    trade_producer.match_order(limit);

    ASSERT_EQ(trade_producer.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(limit.status(), PARTIAL);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 1);
    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), FILLED);
}

TEST(TradeProducer, test_matching_limit_less) {
    auto limit = Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(Price(99), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto trade_producer = TradeProducer(market);
    trade_producer.match_order(limit);

    ASSERT_EQ(trade_producer.remaining_qty().descaled_value(), 1);
    ASSERT_EQ(limit.status(), FILLED);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), PARTIAL);
}