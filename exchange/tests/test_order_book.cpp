#include <gtest/gtest.h>

#include "order_book.h"

TEST(OrderBook, test_simple_trade) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() {};
    ob.order_book_update_handler = [](const LevelUpdate& update) {};
    ob.last_trade_update_handler = []() {};


    ob.trades_update_handler = [&](const Trade& trade) {
        ASSERT_EQ(trade.crossing_side(), SELL);
        ASSERT_EQ(trade.price(), Price(99.7));
    };

    std::vector<Order> limit_orders = {
        Order(Price(99.3), Quantity(1), BUY, OPEN, LIMIT, 555, 1),
        Order(Price(99.5), Quantity(1), BUY, OPEN, LIMIT, 555, 2),
        Order(Price(99.7), Quantity(1), BUY, OPEN, LIMIT, 555, 3),
    };
    for (auto& order : limit_orders) {
        ob.add_order(order);
    }

    std::vector<Order> market_orders = {
        Order(Price(99.5), Quantity(1), SELL, OPEN, MARKET, 9999, 100),
    };

    for (auto& order : market_orders) {
        ob.add_order(order);
    }
}

TEST(OrderBook, test_simple_trade_other_side) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() {};
    ob.order_book_update_handler = [](const LevelUpdate& update) {};
    ob.last_trade_update_handler = []() {};

    ob.trades_update_handler = [&](const Trade& trade) {
        ASSERT_EQ(trade.crossing_side(), BUY);
        ASSERT_EQ(trade.price(), Price(25.2));
    };

    std::vector<Order> limit_orders = {
        Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 1),
        Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 2),
        Order(Price(25.2), Quantity(1), SELL, OPEN, LIMIT, 555, 3),
        Order(Price(25.6), Quantity(1), SELL, OPEN, LIMIT, 555, 2),
    };
    for (auto& order : limit_orders) {
        ob.add_order(order);
    }

    std::vector<Order> market_orders = {
        Order(Price(99.5), Quantity(1), BUY, OPEN, MARKET, 9999, 100),
    };

    for (auto& order : market_orders) {
        ob.add_order(order);
    }
}

TEST(OrderBook, test_simple_remove_order) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() {};
    ob.order_book_update_handler = [](const LevelUpdate& update) {};
    ob.last_trade_update_handler = []() {};

    ob.trades_update_handler = [&](const Trade& trade) {
        ASSERT_EQ(trade.crossing_side(), BUY);
        ASSERT_EQ(trade.price(), Price(25.2));
    };

    std::vector<Order> limit_orders = {
        Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 1),
        Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 2),
    };
    for (auto& order : limit_orders) {
        ob.add_order(order);
    }

    ob.remove_order(1);
}

TEST(OrderBook, test_updates_large_order_placement) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() {};
    ob.order_book_update_handler = [](const LevelUpdate& update) {};
    ob.last_trade_update_handler = []() {};

    ob.trades_update_handler = [&](const Trade& trade) { };

    std::vector<Order> limit_orders = {
        Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 1),
        Order(Price(26.5), Quantity(1), SELL, OPEN, LIMIT, 555, 2),
        Order(Price(27.5), Quantity(1), SELL, OPEN, LIMIT, 555, 3),
        Order(Price(28.5), Quantity(2), SELL, OPEN, LIMIT, 555, 4),
        Order(Price(29.5), Quantity(1), SELL, OPEN, LIMIT, 555, 5),
        Order(Price(30.1), Quantity(1), SELL, OPEN, LIMIT, 555, 6)
    };

    for (auto& order : limit_orders) {
        ob.add_order(order);
    }

    auto market_order = Order(Price(99.5), Quantity(10), BUY, OPEN, LIMIT, 9999, 100);
    auto market_order2 = Order(Price(98.5), Quantity(10), BUY, OPEN, LIMIT, 9999, 101);
    ob.add_order(market_order);
    ob.add_order(market_order2);
}