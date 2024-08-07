#pragma once

#include <functional>

#include "book_side.h"

class OrderBook {
public:
    using BookSideBid = BookSide<std::greater<>>;
    using BookSideAsk = BookSide<std::less<>>;

    using AccountUpdateHandler = std::function<void()>;
    using TradesUpdateHandler = std::function<void(const Trade& trade)>;
    using OrderBookUpdateHandler = std::function<void()>;
    using LastTradeUpdateHandler = std::function<void()>;

    AccountUpdateHandler account_update_handler;
    TradesUpdateHandler trades_update_handler;
    OrderBookUpdateHandler order_book_update_handler;
    LastTradeUpdateHandler last_trade_update_handler;

    OrderBook(Price starting_price, Price tick_size);
    void add_order(Order& order);
    void remove_order(OrderId id);
    void match_order(Order& order);

private:
     BookSideBid bids;
     BookSideAsk asks;
};

