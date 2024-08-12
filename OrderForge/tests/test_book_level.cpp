#include <gtest/gtest.h>

#include "book_level.h"

class TestBookLevel : public ::testing::Test {
public:
    void SetUp() override {
        Logger::get_instance(true, MB * 1, 5);
        REGISTER_TYPE(ORDER, Order);
        REGISTER_TYPE(TRADE, Trade);
    }

private:
    Price bid_price_{50};
    Price ask_price_{100};
    BookLevel book_level_bid_{bid_price_, BUY};
    BookLevel book_level_ask_{ask_price_, SELL};
    std::vector<LevelUpdate> level_updates_;
};

TEST_F(TestBookLevel, test_trades_match_in_correct_order) {

}

TEST(TestBookLevel, match_order) {
    auto pool = std::pmr::unsynchronized_pool_resource();
    Logger::get_instance(true);
    REGISTER_TYPE(ORDER, Order);

    auto bl = BookLevel(Price(100), BUY);

    std::vector<Order> limit_orders = {
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 1),
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 2),
        Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, 3),
    };
    for (auto& order : limit_orders) {
        auto update = bl.add_order(order);
    }

    auto market_order = Order(Price(99), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id());
    auto trade_producer = TradeProducer(market_order, pool);
    bl.match_order(trade_producer);

    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 2);
}

TEST(TestBookLevel, add_order) {
    const Price price(45);
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();
    auto bl = BookLevel(price, BUY);
    auto order = Order(price, qty, BUY, OPEN, LIMIT, 12345, id);

    ASSERT_EQ(bl.total_quantity(), Quantity(0));
    ASSERT_EQ(bl.size(), 0);
    auto update = bl.add_order(order);
    LOG_INFO("{}", update.log_level_update());

    ASSERT_EQ(update.price, price);
    ASSERT_EQ(update.total_quantity, Quantity(5));
    ASSERT_EQ(bl.size(), 1);
    ASSERT_EQ(bl.total_quantity(), qty);
}

TEST(TestBookLevel, remove_order) {
    const Price price(100);
    Quantity qty = Quantity(5);
    const OrderId id = gen_random_order_id();
    auto bl = BookLevel(price, BUY);
    auto order = Order(price, qty, BUY, OPEN, LIMIT, 12345, id);

    auto update = bl.add_order(order);
    ASSERT_EQ(update.price, price);
    ASSERT_EQ(update.total_quantity, Quantity(5));

    ASSERT_EQ(bl.size(), 1);
    ASSERT_EQ(bl.total_quantity(), qty);

    auto update2 = bl.remove_order(id);
    ASSERT_EQ(update2.price, price);
    ASSERT_EQ(update2.total_quantity, Quantity(0));

    ASSERT_EQ(bl.size(), 0);
    ASSERT_EQ(bl.total_quantity(), Quantity(0));
}
