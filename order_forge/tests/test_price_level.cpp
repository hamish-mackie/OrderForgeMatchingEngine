#include <gtest/gtest.h>

#include "price_level.h"

using namespace of;

class TestPriceLevel : public ::testing::Test {
protected:
    void SetUp() override {
        LoggerConfig log_cfg = LoggerConfig{};
        log_cfg.write_std_out = true;
        log_cfg.mem_block_size_mb = of::MB * 1;
        log_cfg.number_blocks = 5;

        Logger::get_instance(log_cfg);
        Logger::register_common_types();
        REGISTER_TYPE(ORDER, Order);
        REGISTER_TYPE(TRADE, Trade);
    }

    std::string_view symbol = "TESTUSD";
    Price bid_price_{50};
    Price ask_price_{100};
    PriceLevel book_level_bid_{bid_price_, BUY};
    PriceLevel book_level_ask_{ask_price_, SELL};
    std::vector<PriceLevelUpdate> level_updates_;
    std::pmr::unsynchronized_pool_resource pool{};
};

TEST_F(TestPriceLevel, match_order) {

    std::vector<Order> limit_orders = {
            Order(symbol, bid_price_, Quantity(1), BUY, OPEN, LIMIT, 9999, 1, 100),
            Order(symbol, bid_price_, Quantity(1), BUY, OPEN, LIMIT, 9999, 2, 200),
            Order(symbol, bid_price_, Quantity(1), BUY, OPEN, LIMIT, 9999, 3, 300),
    };
    for (auto& order: limit_orders) {
        auto update = book_level_bid_.add_order(order);
    }

    auto market_order = Order(symbol, Price(99), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id(),
                              gen_random_order_id());
    auto trade_producer = MatchingEngine(market_order, pool);
    book_level_bid_.match_order(trade_producer);

    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 2);
}

TEST_F(TestPriceLevel, add_order) {
    Quantity qty = Quantity(5);
    const OrderId client_id = gen_random_order_id();
    const OrderId id = gen_random_order_id();
    auto order = Order(symbol, bid_price_, qty, BUY, OPEN, LIMIT, 12345, client_id, id);

    ASSERT_EQ(book_level_bid_.total_quantity(), Quantity(0));
    ASSERT_EQ(book_level_bid_.size(), 0);
    auto update = book_level_bid_.add_order(order);
    // LOG_INFO("{}", update.log_level_update());

    ASSERT_EQ(update.price(), bid_price_);
    ASSERT_EQ(update.total_quantity(), Quantity(5));
    ASSERT_EQ(book_level_bid_.size(), 1);
    ASSERT_EQ(book_level_bid_.total_quantity(), qty);
}

TEST_F(TestPriceLevel, remove_order) {
    Quantity qty = Quantity(5);
    const OrderId client_id = gen_random_order_id();
    const OrderId id = gen_random_order_id();

    auto order = Order(symbol, bid_price_, qty, BUY, OPEN, LIMIT, 12345, client_id, id);

    auto update = book_level_bid_.add_order(order);
    ASSERT_EQ(update.price(), bid_price_);
    ASSERT_EQ(update.total_quantity(), Quantity(5));

    ASSERT_EQ(book_level_bid_.size(), 1);
    ASSERT_EQ(book_level_bid_.total_quantity(), qty);

    auto update2 = book_level_bid_.remove_order(id);
    ASSERT_EQ(update2.price(), bid_price_);
    ASSERT_EQ(update2.total_quantity(), Quantity(0));

    ASSERT_EQ(book_level_bid_.size(), 0);
    ASSERT_EQ(book_level_bid_.total_quantity(), Quantity(0));
}
