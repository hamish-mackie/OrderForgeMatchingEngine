#include "order_book.h"

OrderBook::OrderBook(Price starting_price, Price tick_size)
    : bids(BookSideBid(BUY, tick_size))
    , asks(BookSideAsk(SELL, tick_size)) {
}

void OrderBook::add_order(Order &order) {
    LOG_INFO("OrderBook", order.log_order());

    if(order.type() == MARKET) {
        match_order(order);
    }

    // auto best_bid = bids.best_price();
    // auto best_ask = asks.best_price();

    if(order.type() == LIMIT) {
        if(order.side() == BUY) {
            bids.add_order(order);
        } else {
            asks.add_order(order);
        }
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
    auto trade_producer = TradeProducer(order);
    LOG_INFO("OrderBook", trade_producer.log_producer());
    if(order.side() == BUY) {
        asks.match_order(trade_producer);
    } else {
        bids.match_order(trade_producer);
    }
}
