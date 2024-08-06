#include "order_book.h"

OrderBook::OrderBook(Price starting_price, Price tick_size)
    : bids(BookSide(BUY, starting_price, tick_size))
    , asks(BookSide(SELL, starting_price, tick_size)) {
}

void OrderBook::add_order(const Order &order) {
    LOG_INFO("", order.log_order());
    if(order.side() == BUY) {
        bids.add_order(order);
    } else {
        asks.add_order(order);
    }

    // receive a limit order
    // if the order can be placed in the book without crossing
    // call add order on the same side

    // if the side of the order is a buy, but the best sell price is lower the order will cross
    // generate a trade matcher and match the trades.
    // if there is any quantity left, add that too the book.
}

void OrderBook::remove_order(OrderId id) {

}

void OrderBook::match_order(Order &order) {

}
