#pragma once

#include <functional>

#include "book_side.h"

class OrderBook {
public:
    using BookSideBid = BookSide<std::greater<>>;
    using BookSideAsk = BookSide<std::less<>>;
    using OrdersIdMap = std::unordered_map<OrderId, FindOrderHelper>;

    // Private feed handlers
    using OrderUpdateHandler = std::function<void(const Order& order)>;
    using AccountUpdateHandler = std::function<void()>;
    using TradesUpdateHandler = std::function<void(const Trade& trade)>;
    OrderUpdateHandler private_order_update_handler;
    AccountUpdateHandler private_account_update_handler;
    TradesUpdateHandler private_trades_update_handler;

    // Public feed handlers
    using OrderBookUpdateHandler = std::function<void(const LevelUpdate& update)>;
    using LastTradeUpdateHandler = std::function<void(const LastTradeUpdate&& update)>;
    OrderBookUpdateHandler public_order_book_update_handler;
    LastTradeUpdateHandler public_last_trade_update_handler;

    OrderBook(Price starting_price, Price tick_size);
    void add_order(Order& order);
    void remove_order(OrderId id);
    void match_order(Order& order);

    void add_order_helper(Price price, OrderId order_id, Side side);

private:
    BookSideBid bids;
    BookSideAsk asks;
    OrdersIdMap orders_id_map_;
};

