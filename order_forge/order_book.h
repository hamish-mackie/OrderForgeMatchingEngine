#pragma once

#include "book_side.h"
#include "order_book_config.h"

namespace of {
class OrderBook {
public:
    using BookSideBid = BookSide<std::greater<>>;
    using BookSideAsk = BookSide<std::less<>>;
    using OrdersIdMap = std::unordered_map<OrderId, FindOrderHelper>;
    using LevelUpdates = std::pmr::vector<PriceLevelUpdate>;

    // Private feed handlers
    using OrderUpdateHandler = std::function<void(const Order& order)>;
    using AccountUpdateHandler = std::function<void()>;
    using TradesUpdateHandler = std::function<void(const Trade& trade)>;
    OrderUpdateHandler private_order_update_handler;
    AccountUpdateHandler private_account_update_handler;
    TradesUpdateHandler private_trades_update_handler;

    // Public feed handlers
    using OrderBookUpdateHandler = std::function<void(const PriceLevelUpdate& update)>;
    using LastTradeUpdateHandler = std::function<void(const LastTradeUpdate& update)>;
    OrderBookUpdateHandler public_order_book_update_handler;
    LastTradeUpdateHandler public_last_trade_update_handler;

    explicit OrderBook(OrderBookConfig&);
    void add_order(Order& order);
    void remove_order(OrderId id);

    void add_order_helper(Price price, OrderId order_id, Side side);

private:
    const OrderBookConfig& cfg_;
    std::string symbol_;
    BookSideBid bids;
    BookSideAsk asks;
    OrdersIdMap orders_id_map_;
    std::pmr::unsynchronized_pool_resource pmr_resource_;

    void limit_order(Order& order);
    void market_order(Order& order);
    void fill_and_kill_order(Order& order);

    void match_order(Order& order);
    inline bool is_crossing_order(Order& order);
    void reject_order(Order& order);
};
} // namespace of
