#include <gtest/gtest.h>

#include "book_level.h"

class BookLevelTest : public ::testing::Test {
public:
    void SetUp() override {

    }

private:
    Price bid_price_{50};
    Price ask_price_{100};
    BookLevel book_level_bid_{bid_price_, BUY};
    BookLevel book_level_ask_{ask_price_, SELL};
    std::vector<LevelUpdate> level_updates_;
};

TEST_F(BookLevelTest, test_trades_match_in_correct_order) {

}

TEST(BookLevel, match_order) {
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
    auto trade_producer = TradeProducer(market_order);
    bl.match_order(trade_producer);

    ASSERT_EQ(trade_producer.get_modified_orders_().size(), 2);
}

TEST(BookLevel, add_order) {
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

TEST(BookLevel, remove_order) {
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

// TEST(BookLevel, stress_test_10000_orders_random_removal) {
//     const Price price;
//     const int num_orders = 10000;
//     auto bl = BookLevel(price);
//     std::vector<OrderId> ids;
//     ids.reserve(num_orders);
//
//     for (int i = 0; i < num_orders; ++i) {
//         Quantity qty = Quantity(1);
//         const OrderId id = gen_random_order_id();
//         ids.push_back(id);
//         bl.add_order(Order(price, qty, BUY, OPEN, LIMIT, 12345 + i, id));
//     }
//
//     ASSERT_EQ(bl.size(), num_orders);
//     ASSERT_EQ(bl.total_quantity(), Quantity(num_orders));
//
//     std::random_device rd;
//     std::mt19937 g(rd());
//     std::shuffle(ids.begin(), ids.end(), g);
//
//     for (const auto& id : ids) {
//         bl.remove_order(id);
//     }
//
//     ASSERT_EQ(bl.size(), 0);
//     ASSERT_EQ(bl.total_quantity(), Quantity(0));
// }