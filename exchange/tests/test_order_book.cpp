#include <gtest/gtest.h>

#include "order_book.h"

class OrderBookTest : public ::testing::Test {
public:
    OrderBookTest()
        : start_price_(100)
        , tick_size_(0.01)
        , ob(start_price_, tick_size_) {


    }
protected:
    void SetUp() override {
        ob.private_order_update_handler = [&](const Order& order) { order_updates.push_back(order); };
        ob.private_account_update_handler = [&]() {};
        ob.private_trades_update_handler = [&](const Trade& trade) { trade_updates_.push_back(trade); };
        ob.public_order_book_update_handler = [&](const LevelUpdate& update) { level_updates_.push_back(update); };
        ob.public_last_trade_update_handler = [&](const LastTradeUpdate& update) { last_trade_updates_.push_back(update); };
    }

    void TearDown() override {

    }
    Price start_price_;
    Price tick_size_;
    OrderBook ob;
    std::vector<Order> order_updates;
    std::vector<LevelUpdate> level_updates_;
    std::vector<Trade> trade_updates_;
    std::vector<LastTradeUpdate> last_trade_updates_;
};

void verify_order(Order& sent, Order& received) {
    ASSERT_EQ(sent.price().descaled_value(), received.price().descaled_value());
    ASSERT_EQ(sent.qty().descaled_value(), received.qty().descaled_value());
    ASSERT_EQ(sent.status(), received.status());
    ASSERT_EQ(sent.type(), received.type());
    ASSERT_EQ(sent.side(), received.side());
    ASSERT_EQ(sent.order_id(), received.order_id());
}

void verify_level_update(LevelUpdate& update, Price price, Quantity total_qty, Side side) {
    ASSERT_EQ(update.price.descaled_value(), price.descaled_value());
    ASSERT_EQ(update.total_quantity.descaled_value(), total_qty.descaled_value());
    ASSERT_EQ(update.side, side);
}

TEST_F(OrderBookTest, test_send_limit_orders) {
    auto order1 = Order(Price(99.3), Quantity(1), BUY, OPEN, LIMIT, 555, 1);
    ob.add_order(order1);
    verify_order(order1, order_updates[0]);
    verify_level_update(level_updates_[0], order1.price(), order1.qty(), order1.side());
}

TEST(OrderBookTest, test_send_market_orders) {


}



TEST(OrderBook, test_simple_trade) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.public_order_book_update_handler = [](const LevelUpdate& update) {};


    ob.private_trades_update_handler = [&](const Trade& trade) {
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
    ob.private_account_update_handler = []() {};
    ob.public_order_book_update_handler = [](const LevelUpdate& update) {};

    ob.private_trades_update_handler = [&](const Trade& trade) {
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
    ob.private_account_update_handler = []() {};
    ob.public_order_book_update_handler = [](const LevelUpdate& update) {};

    ob.private_trades_update_handler = [&](const Trade& trade) {
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
    ob.private_order_update_handler = [](const Order& order) {};
    ob.private_account_update_handler = []() {};
    ob.private_trades_update_handler = [&](const Trade& trade) { };
    ob.public_order_book_update_handler = [](const LevelUpdate& update) {};
    ob.public_last_trade_update_handler = [](const LastTradeUpdate& update) {};

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

TEST(OrderBook, test_send_limits) {
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    std::vector<Order> order_vec;
    std::vector<LevelUpdate> book_update_vec;
    ob.private_order_update_handler = [&](const Order& order) {order_vec.push_back(order); };
    ob.public_order_book_update_handler = [&](const LevelUpdate& update) {book_update_vec.push_back(update); };

    auto order1 = Order(Price(25.5), Quantity(1), SELL, OPEN, LIMIT, 555, 1);
    ob.add_order(order1);
    auto order2 = Order(Price(25.5), Quantity(3), SELL, OPEN, LIMIT, 555, 1);
    ob.add_order(order2);

    ASSERT_EQ(order_vec[0].status(), OPEN);
    ASSERT_EQ(order_vec[0].price().descaled_value(), 25.5);
    ASSERT_EQ(book_update_vec[0].price.descaled_value(), 25.5);
    ASSERT_EQ(book_update_vec[0].total_quantity.descaled_value(), 1);

    ASSERT_EQ(order_vec[1].status(), OPEN);
    ASSERT_EQ(order_vec[1].price().descaled_value(), 25.5);
    ASSERT_EQ(book_update_vec[1].price.descaled_value(), 25.5);
    ASSERT_EQ(book_update_vec[1].total_quantity.descaled_value(), 4);
}