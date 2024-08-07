#include <gtest/gtest.h>

#include "order_book.h"

class OrderBookTest : public ::testing::Test {
public:
    OrderBookTest()
        : start_price_(100)
        , tick_size_(0.01)
        , ob(start_price_, tick_size_) {}
protected:
    void SetUp() override {
        ob.private_order_update_handler = [&](const Order& order) { order_updates.push_back(order); };
        ob.private_account_update_handler = [&]() {};
        ob.private_trades_update_handler = [&](const Trade& trade) { trade_updates_.push_back(trade); };
        ob.public_order_book_update_handler = [&](const LevelUpdate& update) { level_updates_.push_back(update); };
        ob.public_last_trade_update_handler = [&](const LastTradeUpdate& update) { last_trade_updates_.push_back(update); };
    }

    Price start_price_;
    Price tick_size_;
    OrderBook ob;
    std::vector<Order> order_updates;
    std::vector<LevelUpdate> level_updates_;
    std::vector<Trade> trade_updates_;
    std::vector<LastTradeUpdate> last_trade_updates_;
};

void verify_order(Order& result, Order& received) {
    ASSERT_EQ(result.price().descaled_value(), received.price().descaled_value());
    // check sent qty vs remaining qty so we can make a result order to test against.
    ASSERT_EQ(result.qty().descaled_value(), received.remaining_qty().descaled_value());
    ASSERT_EQ(result.status(), received.status());
    ASSERT_EQ(result.type(), received.type());
    ASSERT_EQ(result.side(), received.side());
    ASSERT_EQ(result.order_id(), received.order_id());
}

void verify_level_update(LevelUpdate& result, LevelUpdate& received) {
    ASSERT_EQ(result.price.descaled_value(), received.price.descaled_value());
    ASSERT_EQ(result.total_quantity.descaled_value(), received.total_quantity.descaled_value());
    ASSERT_EQ(result.side, received.side);
}

void verify_level_update(LevelUpdate& result, Price price, Quantity total_qty, Side side) {
    auto received = LevelUpdate(price, total_qty, side);
    verify_level_update(result, received);
}

TEST_F(OrderBookTest, test_send_and_remove_limit_orders) {
    std::vector<Order> orders = {
        Order(Price(99.3),  Quantity(0.1),  BUY,  OPEN, LIMIT, 555, 1),
        Order(Price(99.5),  Quantity(0.2),  BUY,  OPEN, LIMIT, 555, 2),
        Order(Price(99.1),  Quantity(1),    BUY,  OPEN, LIMIT, 555, 3),
        Order(Price(100),   Quantity(1),    SELL, OPEN, LIMIT, 555, 4),
        Order(Price(100.05),Quantity(5),    SELL, OPEN, LIMIT, 555, 5),
        Order(Price(100.6), Quantity(8),    SELL, OPEN, LIMIT, 555, 6)
    };

    std::vector<LevelUpdate> expected_level_updates = {
        LevelUpdate(Price(99.3),    Quantity(0.1),  BUY),
        LevelUpdate(Price(99.5),    Quantity(0.2),  BUY),
        LevelUpdate(Price(99.1),    Quantity(1),    BUY),
        LevelUpdate(Price(100),     Quantity(1),    SELL),
        LevelUpdate(Price(100.05),  Quantity(5),    SELL),
        LevelUpdate(Price(100.6),   Quantity(8),    SELL),
        LevelUpdate(Price(99.3),    Quantity(0),    BUY),
        LevelUpdate(Price(99.5),    Quantity(0),    BUY),
        LevelUpdate(Price(99.1),    Quantity(0),    BUY),
        LevelUpdate(Price(100),     Quantity(0),    SELL),
        LevelUpdate(Price(100.05),  Quantity(0),    SELL),
        LevelUpdate(Price(100.6),   Quantity(0),    SELL)
    };

    for(auto i = 0; i < orders.size(); ++i) {
        auto order = orders[i];
        ob.add_order(order);
        verify_order(order, order_updates[i]);
        verify_level_update(level_updates_[i], order.price(), order.qty(), order.side());
    }
}

TEST_F(OrderBookTest, test_send_market_orders) {
    std::vector<Order> orders = {
        Order(Price(98),    Quantity(5), BUY,  OPEN, LIMIT, 555, 1),
        Order(Price(99),    Quantity(5), BUY,  OPEN, LIMIT, 555, 2),
        Order(Price(97),    Quantity(5), BUY,  OPEN, LIMIT, 555, 3),
        Order(Price(100),   Quantity(5), SELL, OPEN, LIMIT, 555, 4),
        Order(Price(101),   Quantity(5), SELL, OPEN, LIMIT, 555, 5),
        Order(Price(105),   Quantity(5), SELL, OPEN, LIMIT, 555, 6)
    };

    std::vector<LevelUpdate> expected_level_updates {
        LevelUpdate(Price(98),  Quantity(5), BUY),
        LevelUpdate(Price(99),  Quantity(5), BUY),
        LevelUpdate(Price(97),  Quantity(5), BUY),
        LevelUpdate(Price(100), Quantity(5), SELL),
        LevelUpdate(Price(101), Quantity(5), SELL),
        LevelUpdate(Price(105), Quantity(5), SELL),

        // Sell Order
        LevelUpdate(Price(100), Quantity(0), SELL),
        LevelUpdate(Price(101), Quantity(0), SELL),
        LevelUpdate(Price(105), Quantity(0), SELL),
        LevelUpdate(Price(106), Quantity(5), BUY),
        LevelUpdate(Price(106), Quantity(0), BUY),

        // Buy O
        LevelUpdate(Price(99),  Quantity(0), BUY),
        LevelUpdate(Price(98),  Quantity(0), BUY),
        LevelUpdate(Price(97),  Quantity(0), BUY),
        LevelUpdate(Price(90),  Quantity(85),SELL),
        LevelUpdate(Price(90),  Quantity(0), SELL)
    };

    for(auto i = 0; i < orders.size(); ++i) {
        auto order = orders[i];
        ob.add_order(order);
        verify_order(order, order_updates[i]);
        verify_level_update(level_updates_[i], order.price(), order.qty(), order.side());
    }

    auto market_buy = Order(Price(106), Quantity(20), BUY, OPEN, LIMIT, 555, 8);
    auto market_buy_result = Order(Price(106), Quantity(5), BUY, PARTIAL, LIMIT, 555, 8);
    ob.add_order(market_buy);
    verify_order(market_buy_result, order_updates.back());
    ob.remove_order(8);

    auto market_sell = Order(Price(90), Quantity(100), SELL, OPEN, LIMIT, 555, 9);
    auto market_sell_result = Order(Price(90), Quantity(85), SELL, PARTIAL, LIMIT, 555, 9);
    ob.add_order(market_sell);
    verify_order(market_sell_result, order_updates.back());
    ob.remove_order(9);

    ASSERT_EQ(expected_level_updates.size(), level_updates_.size());

    for(int i = 0; i < expected_level_updates.size(); ++i) {
        verify_level_update(expected_level_updates[i], level_updates_[i]);
    }
}