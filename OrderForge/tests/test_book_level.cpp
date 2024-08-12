#include <gtest/gtest.h>

#include "book_level.h"

class TestBookLevel : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::get_instance(true, MB * 1, 5);
        REGISTER_TYPE(ORDER, Order);
        REGISTER_TYPE(TRADE, Trade);
        REGISTER_TYPE(DEBUG, Debug);
    }

    Price bid_price_{50};
    Price ask_price_{100};
    BookLevel book_level_bid_{bid_price_, BUY};
    BookLevel book_level_ask_{ask_price_, SELL};
    std::vector<LevelUpdate> level_updates_;
    std::pmr::unsynchronized_pool_resource pool{};
};

TEST_F(TestBookLevel, match_order) {

    std::vector<Order> limit_orders = {
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 1),
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 2),
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 3),
    };
    for (auto& order : limit_orders) {
        auto update = book_level_bid_.add_order(order);
    }

    auto market_order = Order(Price(99), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id());
    auto trade_producer = TradeProducer(market_order, pool);
    book_level_bid_.match_order(trade_producer);

    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 2);
}

TEST_F(TestBookLevel, add_order) {
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();
    auto order = Order(bid_price_, qty, BUY, OPEN, LIMIT, 12345, id);

    ASSERT_EQ(book_level_bid_.total_quantity(), Quantity(0));
    ASSERT_EQ(book_level_bid_.size(), 0);
    auto update = book_level_bid_.add_order(order);
    LOG_INFO("{}", update.log_level_update());

    ASSERT_EQ(update.price, bid_price_);
    ASSERT_EQ(update.total_quantity, Quantity(5));
    ASSERT_EQ(book_level_bid_.size(), 1);
    ASSERT_EQ(book_level_bid_.total_quantity(), qty);
}

TEST_F(TestBookLevel, remove_order) {
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();

    auto order = Order(bid_price_, qty, BUY, OPEN, LIMIT, 12345, id);

    auto update = book_level_bid_.add_order(order);
    ASSERT_EQ(update.price, bid_price_);
    ASSERT_EQ(update.total_quantity, Quantity(5));

    ASSERT_EQ(book_level_bid_.size(), 1);
    ASSERT_EQ(book_level_bid_.total_quantity(), qty);

    auto update2 = book_level_bid_.remove_order(id);
    ASSERT_EQ(update2.price, bid_price_);
    ASSERT_EQ(update2.total_quantity, Quantity(0));

    ASSERT_EQ(book_level_bid_.size(), 0);
    ASSERT_EQ(book_level_bid_.total_quantity(), Quantity(0));
}
