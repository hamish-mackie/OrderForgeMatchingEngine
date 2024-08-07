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

    // TODO implement checking if a limit will fill
    // TODO test limits get matched

    if(order.type() == LIMIT) {
        if(order.side() == BUY) {

            auto best_price = asks.best_price();
            if(best_price.has_value() && best_price <= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                bids.add_order(order);
            }
        } else {
            auto best_price = bids.best_price();
            if(best_price.has_value() && best_price >= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                asks.add_order(order);
            }
        }
    }
}

void OrderBook::remove_order(OrderId id) {
    // TODO add remove order
}

void OrderBook::match_order(Order &order) {
    auto trade_producer = TradeProducer(order);
    LOG_INFO("OrderBook", trade_producer.log_producer());
    if(order.side() == BUY) {
        asks.match_order(trade_producer);
    } else {
        bids.match_order(trade_producer);
    }

    for(auto& trade: trade_producer.get_trades()) {
        LOG_INFO(trade.log_trade());
        trades_update_handler(trade);
    }
}
