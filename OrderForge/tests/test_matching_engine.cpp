#include <gtest/gtest.h>

#include "matching_engine.h"

using namespace of;

static constexpr std::string_view symbol = "TESTUSD";

TEST(TestMatchingEngine, test_matching_same_size) {

    auto pool = std::pmr::unsynchronized_pool_resource();
    auto limit = Order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(symbol, Price(99), Quantity(1), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto matching_engine = MatchingEngine(market, pool);
    matching_engine.match_order(limit);

    ASSERT_EQ(matching_engine.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(limit.status(), FILLED);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(matching_engine.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), FILLED);
}

TEST(TestMatchingEngine, test_matching_limit_greater) {
    auto pool = std::pmr::unsynchronized_pool_resource();
    auto limit = Order(symbol, Price(100), Quantity(2), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(symbol, Price(99), Quantity(1), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto matching_engine = MatchingEngine(market, pool);
    matching_engine.match_order(limit);

    ASSERT_EQ(matching_engine.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(limit.status(), PARTIAL);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 1);
    ASSERT_EQ(matching_engine.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), FILLED);
}

TEST(TestMatchingEngine, test_matching_limit_less) {
    auto pool = std::pmr::unsynchronized_pool_resource();
    auto limit = Order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    auto market = Order(symbol, Price(99), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id());

    auto matching_engine = MatchingEngine(market, pool);
    matching_engine.match_order(limit);

    ASSERT_EQ(matching_engine.remaining_qty().descaled_value(), 1);
    ASSERT_EQ(limit.status(), FILLED);
    ASSERT_EQ(limit.remaining_qty().descaled_value(), 0);
    ASSERT_EQ(matching_engine.get_modified_orders_().size(), 1);
    ASSERT_EQ(market.status(), PARTIAL);
}
