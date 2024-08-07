#include "order_book.h"

OrderBook::OrderBook(Price starting_price, Price tick_size)
    : bids(BookSideBid(BUY, tick_size))
    , asks(BookSideAsk(SELL, tick_size)) {
}

void OrderBook::add_order(Order &order) {
    LOG_INFO("OrderBook", order.log_order());

    std::vector<LevelUpdate> updates;

    if(order.type() == MARKET) {
        match_order(order);
    }

    // TODO show orderbook changes after insert and removal

    if(order.type() == LIMIT) {
        if(order.side() == BUY) {
            auto best_price = asks.best_price();
            if(best_price.has_value() && best_price <= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                updates.push_back(bids.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
            }
        } else {
            auto best_price = bids.best_price();
            if(best_price.has_value() && best_price >= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                updates.push_back(asks.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
            }
        }
    }

    for(auto update: updates) {
        LOG_INFO(update.log_level_update());
    }
}

void OrderBook::remove_order(OrderId id) {
    LOG_INFO(id);
    std::vector<LevelUpdate> updates;
    auto it = orders_id_map_.find(id);
    if(it != orders_id_map_.end()) {
        auto helper = it->second;
        if(helper.side == BUY) {
            updates.push_back(bids.remove_order(helper));
        } else {
            updates.push_back(asks.remove_order(helper));
        }
    }

    for(auto update: updates) {
        LOG_INFO(update.log_level_update());
        order_book_update_handler(update);
    }
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

void OrderBook::add_order_helper(Price price, OrderId order_id, Side side) {
    auto helper = FindOrderHelper(order_id, side, price);
    orders_id_map_.emplace(order_id, std::move(helper));
}
