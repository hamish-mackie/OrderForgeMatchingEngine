#include <gtest/gtest.h>

#include "order_book.h"

using namespace of;

class TestOrderBook : public ::testing::Test {
public:
    TestOrderBook() : start_price_(100), tick_size_(0.01), cfg(symbol.data(), tick_size_), ob(cfg) {}

protected:
    void SetUp() override {
        ob.private_order_update_handler = [&](const Order& order) { order_handler(order); };
        ob.private_account_update_handler = [&]() {};
        ob.private_trades_update_handler = [&](const Trade& trade) { trade_handler(trade); };
        ob.public_order_book_update_handler = [&](const PriceLevelUpdate& update) { level_update_handler(update); };
        ob.public_last_trade_update_handler = [&](const LastTradeUpdate& update) { last_trade_handler(update); };
    }
    std::string_view symbol = "TESTUSD";
    Price start_price_;
    Price tick_size_;
    OrderBookConfig cfg;
    OrderBook ob;
    std::vector<Order> order_updates;
    std::vector<PriceLevelUpdate> level_updates_;
    std::vector<Trade> trade_updates_;
    std::vector<LastTradeUpdate> last_trade_updates_;

    void order_handler(const Order& order) {
        fmt::println("Order: Price {}, Quantity {}/{}, Side: {}, Status: {}", order.price(), order.remaining_qty(),
                     order.qty(), enum_str(order.side()), enum_str(order.status()));
        order_updates.push_back(order);
    }

    void trade_handler(const Trade& trade) {
        fmt::println("Trade: Price {}, Quantity {}, Side: {}", trade.price(), trade.qty(),
                     enum_str(trade.crossing_side()));
        trade_updates_.push_back(trade);
    }

    void level_update_handler(const PriceLevelUpdate& update) {
        fmt::println("Level Update: Price: {}, Quantity: {}, Side: {}", update.price(), update.total_quantity(),
                     enum_str(update.side()));
        level_updates_.push_back(update);
    }

    void last_trade_handler(const LastTradeUpdate& update) {
        fmt::println("Last Trade: Price: {}, Quantity: {}, Side: {}", update.price(), update.quantity(),
                     enum_str(update.side()));
        last_trade_updates_.push_back(update);
    }
};

void verify_order_update(Order& result, Order& received) {
    ASSERT_EQ(result.price().descaled_value(), received.price().descaled_value());
    // check sent qty vs remaining qty so we can make a result order to test against.
    ASSERT_EQ(result.qty().descaled_value(), received.remaining_qty().descaled_value());
    ASSERT_EQ(result.status(), received.status());
    ASSERT_EQ(result.type(), received.type());
    ASSERT_EQ(result.side(), received.side());
    if (result.order_id() == 0) {
        ASSERT_NE(result.order_id(), received.order_id());
    } else {
        ASSERT_EQ(result.order_id(), received.order_id());
    }
    ASSERT_EQ(result.client_order_id(), received.client_order_id());
}

void verify_level_update(PriceLevelUpdate& result, PriceLevelUpdate& received) {
    ASSERT_EQ(result.price(), received.price());
    ASSERT_EQ(result.total_quantity(), received.total_quantity());
    ASSERT_EQ(result.side(), received.side());
}

void verify_level_update(PriceLevelUpdate& result, Symbol symbol, Price price, Quantity total_qty, Side side) {
    auto received = PriceLevelUpdate(symbol, price, total_qty, side);
    verify_level_update(result, received);
}

void verify_trade_update(Trade& result, Trade& received) {
    ASSERT_EQ(result.price().descaled_value(), received.price().descaled_value());
    ASSERT_EQ(result.qty().descaled_value(), received.qty().descaled_value());
    ASSERT_EQ(result.crossing_side(), received.crossing_side());
    ASSERT_EQ(result.passive_order_id(), received.passive_order_id());
    ASSERT_EQ(result.crossing_order_id(), received.crossing_order_id());
    ASSERT_EQ(result.passive_account_id(), received.passive_account_id());
    ASSERT_EQ(result.crossing_account_id(), received.crossing_account_id());
}

void verify_last_trade_update(LastTradeUpdate& result, LastTradeUpdate& received) {
    ASSERT_EQ(result.price().descaled_value(), received.price().descaled_value());
    ASSERT_EQ(result.quantity().descaled_value(), received.quantity().descaled_value());
    ASSERT_EQ(result.side(), received.side());
}


TEST_F(TestOrderBook, test_send_and_remove_limit_orders) {
    std::vector<Order> orders = {Order(symbol, Price(99.3), Quantity(0.1), BUY, OPEN, LIMIT, 555, 1),
                                 Order(symbol, Price(99.5), Quantity(0.2), BUY, OPEN, LIMIT, 555, 2),
                                 Order(symbol, Price(99.1), Quantity(1), BUY, OPEN, LIMIT, 555, 3),
                                 Order(symbol, Price(100), Quantity(1), SELL, OPEN, LIMIT, 555, 4),
                                 Order(symbol, Price(100.05), Quantity(5), SELL, OPEN, LIMIT, 555, 5),
                                 Order(symbol, Price(100.6), Quantity(8), SELL, OPEN, LIMIT, 555, 6)};

    std::vector<PriceLevelUpdate> expected_level_updates = {
            PriceLevelUpdate(symbol, Price(99.3), Quantity(0.1), BUY),  PriceLevelUpdate(symbol, Price(99.5), Quantity(0.2), BUY),
            PriceLevelUpdate(symbol, Price(99.1), Quantity(1), BUY),    PriceLevelUpdate(symbol, Price(100), Quantity(1), SELL),
            PriceLevelUpdate(symbol, Price(100.05), Quantity(5), SELL), PriceLevelUpdate(symbol, Price(100.6), Quantity(8), SELL),
            PriceLevelUpdate(symbol, Price(99.3), Quantity(0), BUY),    PriceLevelUpdate(symbol, Price(99.5), Quantity(0), BUY),
            PriceLevelUpdate(symbol, Price(99.1), Quantity(0), BUY),    PriceLevelUpdate(symbol, Price(100), Quantity(0), SELL),
            PriceLevelUpdate(symbol, Price(100.05), Quantity(0), SELL), PriceLevelUpdate(symbol, Price(100.6), Quantity(0), SELL)};

    for (auto i = 0; i < orders.size(); ++i) {
        auto order = orders[i];
        ob.add_order(order);
        verify_order_update(order, order_updates[i]);
        verify_level_update(level_updates_[i], symbol,  order.price(), order.qty(), order.side());
    }

    std::vector<OrderId> update_order_ids;
    for (auto& update: order_updates) {
        update_order_ids.push_back(update.order_id());
    }

    for (auto& order_id: update_order_ids) {
        ob.remove_order(order_id);
    }

    ASSERT_EQ(expected_level_updates.size(), level_updates_.size());

    for (auto i = 0; i < expected_level_updates.size(); ++i) {
        verify_level_update(expected_level_updates[i], level_updates_[i]);
    }
}

TEST_F(TestOrderBook, test_send_market_orders) {
    auto crossing_acc_id = 100;
    auto passive_acc_id = 555;
    auto buy_order_id = 50;
    auto sell_order_id = 51;
    std::vector<Order> orders = {Order(symbol, Price(98), Quantity(5), BUY, OPEN, LIMIT, passive_acc_id, 10, 1),
                                 Order(symbol, Price(99), Quantity(5), BUY, OPEN, LIMIT, passive_acc_id, 20, 2),
                                 Order(symbol, Price(97), Quantity(5), BUY, OPEN, LIMIT, passive_acc_id, 30, 3),
                                 Order(symbol, Price(100), Quantity(5), SELL, OPEN, LIMIT, passive_acc_id, 40, 4),
                                 Order(symbol, Price(101), Quantity(5), SELL, OPEN, LIMIT, passive_acc_id, 50, 5),
                                 Order(symbol, Price(105), Quantity(5), SELL, OPEN, LIMIT, passive_acc_id, 60, 6)};

    std::vector<LastTradeUpdate> expected_last_trade_updates{
            // Buy Order
            LastTradeUpdate(symbol, Price(100), Quantity(5), BUY),
            LastTradeUpdate(symbol, Price(101), Quantity(5), BUY),
            LastTradeUpdate(symbol, Price(105), Quantity(5), BUY),
            // Sell Order
            LastTradeUpdate(symbol, Price(99), Quantity(5), SELL),
            LastTradeUpdate(symbol, Price(98), Quantity(5), SELL),
            LastTradeUpdate(symbol, Price(97), Quantity(5), SELL),
    };

    std::vector<Trade> expected_trade_updates{
            // Buy order
            Trade(symbol, Price(100), Quantity(5), BUY, passive_acc_id, crossing_acc_id, 4, buy_order_id),
            Trade(symbol, Price(101), Quantity(5), BUY, passive_acc_id, crossing_acc_id, 5, buy_order_id),
            Trade(symbol, Price(105), Quantity(5), BUY, passive_acc_id, crossing_acc_id, 6, buy_order_id),
            // Sell order
            Trade(symbol, Price(99), Quantity(5), SELL, passive_acc_id, crossing_acc_id, 2, sell_order_id),
            Trade(symbol, Price(98), Quantity(5), SELL, passive_acc_id, crossing_acc_id, 1, sell_order_id),
            Trade(symbol, Price(97), Quantity(5), SELL, passive_acc_id, crossing_acc_id, 3, sell_order_id),
    };

    std::vector<PriceLevelUpdate> expected_level_updates{
            PriceLevelUpdate(symbol, Price(98), Quantity(5), BUY), PriceLevelUpdate(symbol, Price(99), Quantity(5), BUY),
            PriceLevelUpdate(symbol, Price(97), Quantity(5), BUY), PriceLevelUpdate(symbol, Price(100), Quantity(5), SELL),
            PriceLevelUpdate(symbol, Price(101), Quantity(5), SELL), PriceLevelUpdate(symbol, Price(105), Quantity(5), SELL),

            // Sell Order
            PriceLevelUpdate(symbol, Price(100), Quantity(0), SELL), PriceLevelUpdate(symbol, Price(101), Quantity(0), SELL),
            PriceLevelUpdate(symbol, Price(105), Quantity(0), SELL), PriceLevelUpdate(symbol, Price(106), Quantity(5), BUY),
            PriceLevelUpdate(symbol, Price(106), Quantity(0), BUY),

            // Buy O
            PriceLevelUpdate(symbol, Price(99), Quantity(0), BUY), PriceLevelUpdate(symbol, Price(98), Quantity(0), BUY),
            PriceLevelUpdate(symbol, Price(97), Quantity(0), BUY), PriceLevelUpdate(symbol, Price(90), Quantity(85), SELL),
            PriceLevelUpdate(symbol, Price(90), Quantity(0), SELL)};

    for (auto i = 0; i < orders.size(); ++i) {
        auto order = orders[i];
        ob.add_order(order);
        verify_order_update(order, order_updates[i]);
        verify_level_update(level_updates_[i], symbol, order.price(), order.qty(), order.side());
    }

    auto market_buy = Order(symbol, Price(106), Quantity(20), BUY, OPEN, LIMIT, crossing_acc_id, 100, buy_order_id);
    auto market_buy_result =
            Order(symbol, Price(106), Quantity(5), BUY, PARTIAL, LIMIT, crossing_acc_id, 100, buy_order_id);
    ob.add_order(market_buy);
    verify_order_update(market_buy_result, order_updates.back());
    ob.remove_order(buy_order_id);

    auto market_sell = Order(symbol, Price(90), Quantity(100), SELL, OPEN, LIMIT, crossing_acc_id, 100, sell_order_id);
    auto market_sell_result =
            Order(symbol, Price(90), Quantity(85), SELL, PARTIAL, LIMIT, crossing_acc_id, 100, sell_order_id);
    ob.add_order(market_sell);
    verify_order_update(market_sell_result, order_updates.back());
    ob.remove_order(sell_order_id);

    ASSERT_EQ(expected_level_updates.size(), level_updates_.size());

    for (int i = 0; i < expected_level_updates.size(); ++i) {
        verify_level_update(expected_level_updates[i], level_updates_[i]);
    }

    ASSERT_EQ(expected_trade_updates.size(), trade_updates_.size());

    for (int i = 0; i < expected_trade_updates.size(); ++i) {
        verify_trade_update(expected_trade_updates[i], trade_updates_[i]);
    }

    ASSERT_EQ(expected_last_trade_updates.size(), last_trade_updates_.size());

    for (int i = 0; i < expected_last_trade_updates.size(); ++i) {
        verify_last_trade_update(expected_last_trade_updates[i], last_trade_updates_[i]);
    }
}

TEST_F(TestOrderBook, test_order_id_assignment) {
    // send an order without an order id, verify it has one when it comes back.
    uint64_t order_id = 0;
    Order input_order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 555, 10, order_id);
    Order input_order2(symbol, Price(101), Quantity(1), SELL, OPEN, LIMIT, 555, 10, order_id);
    ob.add_order(input_order);
    auto& order1 = order_updates.back();
    ob.add_order(input_order2);
    auto& order2 = order_updates.back();

    ASSERT_NE(order_id, order1.order_id());
    ASSERT_NE(order_id, order2.order_id());
    ASSERT_NE(order1.order_id(), order2.order_id());
}

TEST_F(TestOrderBook, test_trade_generated_with_trade_id) {
    Order buy_order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 555, 1);
    Order sell_order(symbol, Price(100), Quantity(1), SELL, OPEN, LIMIT, 556, 2);

    ob.add_order(buy_order);
    ob.add_order(sell_order);

    ASSERT_FALSE(trade_updates_.empty());
    ASSERT_NE(trade_updates_.back().trade_id(), 0);
}

TEST_F(TestOrderBook, test_badly_price_market_still_matches) {
    Order buy_order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 555, 1);
    Order sell_order(symbol, Price(101), Quantity(1), SELL, OPEN, MARKET, 556, 2);

    ob.add_order(buy_order);
    ob.add_order(sell_order);

    ASSERT_FALSE(trade_updates_.empty());
    ASSERT_NE(trade_updates_.back().trade_id(), 0);
}

TEST_F(TestOrderBook, test_limit_order_stops_filling_on_cross) {
    std::vector<Order> orders{Order(symbol, Price(100), Quantity(1), BUY, OPEN, LIMIT, 555),
                              Order(symbol, Price(101), Quantity(1), BUY, OPEN, LIMIT, 556),
                              Order(symbol, Price(101), Quantity(1), SELL, OPEN, LIMIT, 556)};

    for (auto order: orders) {
        ob.add_order(order);
    }

    ASSERT_EQ(trade_updates_.size(), 1);
    ASSERT_EQ(level_updates_.size(), 3);
}

TEST_F(TestOrderBook, test_market_order_into_empty_book) {
    Order sell_order(symbol, Price(101), Quantity(1), SELL, OPEN, MARKET, 556, 2);
    Order buy_order(symbol, Price(101), Quantity(1), BUY, OPEN, MARKET, 556, 2);

    ob.add_order(sell_order);
    ob.add_order(buy_order);

    ASSERT_EQ(order_updates.size(), 2);
    for (auto& order: order_updates) {
        ASSERT_EQ(order.status(), REJECTED);
    }
}

TEST_F(TestOrderBook, test_fak_order_fully_filled_with_liquidity) {
    std::vector<Order> initial_orders = {Order(symbol, Price(100), Quantity(5), SELL, OPEN, LIMIT, 555)};

    for (auto& order: initial_orders) {
        ob.add_order(order);
    }

    // FAK Buy Order
    Order fak_buy_order(symbol, Price(100), Quantity(5), BUY, OPEN, FILL_AND_KILL, 555);
    ob.add_order(fak_buy_order);

    ASSERT_EQ(trade_updates_.size(), 1);
    ASSERT_EQ(trade_updates_.back().qty().descaled_value(), 5);
    ASSERT_EQ(order_updates.size(), 3);
    ASSERT_EQ(order_updates.back().status(), FILLED);

    ASSERT_EQ(level_updates_.back().total_quantity(), Quantity(0));
}


TEST_F(TestOrderBook, test_fak_order_partial_fill_and_cancel) {
    std::vector<Order> initial_orders = {Order(symbol, Price(100), Quantity(3), SELL, OPEN, LIMIT, 555, 1)};

    for (auto& order: initial_orders) {
        ob.add_order(order);
    }

    Order fak_buy_order(symbol, Price(101), Quantity(5), BUY, OPEN, FILL_AND_KILL, 555, 2);
    ob.add_order(fak_buy_order);

    ASSERT_EQ(trade_updates_.size(), 1);
    ASSERT_EQ(trade_updates_.back().qty().descaled_value(), 3);
    ASSERT_EQ(order_updates[1].status(), FILLED);
    ASSERT_EQ(order_updates[2].status(), PARTIAL);

    ASSERT_EQ(order_updates[1].remaining_qty().descaled_value(), 0);
    ASSERT_EQ(order_updates[2].remaining_qty().descaled_value(), 2);

    ASSERT_EQ(level_updates_[1].total_quantity(), Quantity(0));
}

TEST_F(TestOrderBook, test_fak_wont_fill_at_all) {
    Order sell_order(symbol, Price(101), Quantity(1), SELL, OPEN, LIMIT, 556, 2);
    Order buy_order(symbol, Price(100), Quantity(1), BUY, OPEN, FILL_AND_KILL, 556, 2);

    ob.add_order(sell_order);
    ob.add_order(buy_order);

    ASSERT_EQ(order_updates.size(), 2);

    ASSERT_EQ(order_updates[0].status(), OPEN);
    ASSERT_EQ(order_updates[1].status(), REJECTED);
}
