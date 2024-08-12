#include "order_book.h"

OrderBook::OrderBook(Price starting_price, TickSize tick_size)
    : bids(BookSideBid(BUY, tick_size))
    , asks(BookSideAsk(SELL, tick_size)) {

    Logger::get_instance(false, MB * 50, 5);
    REGISTER_TYPE(ORDER, Order);
    REGISTER_TYPE(TRADE, Trade);
    REGISTER_TYPE(DEBUG, Debug);
    // Call order allocator, so it allocates up front.
    SingleTonWrapper<PoolAllocator<Node<OrderId, Order>>>::get_instance(131072);
}

void OrderBook::add_order(Order &order) {
    LevelUpdates updates;

    if(order.type() == MARKET) {
        match_order(order);
    }

    if(order.type() == LIMIT) {
        if(order.side() == BUY) {
            auto best_price = asks.best_price();
            if(best_price.has_value() && best_price <= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                updates.push_back(bids.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
                if(private_order_update_handler) {
                    private_order_update_handler(order);
                }
            }
        } else {
            auto best_price = bids.best_price();
            if(best_price.has_value() && best_price >= order.price()) {
                match_order(order);
            }
            if(order.remaining_qty().value() > 0) {
                updates.push_back(asks.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
                if(private_order_update_handler) {
                    private_order_update_handler(order);
                }
            }
        }
    }

    for(auto& update: updates) {
        LOG_DEBUG("{}", update.log_level_update());
        if(public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::remove_order(OrderId id) {
    LOG_DEBUG("{}", id);
    LevelUpdates updates;
    auto it = orders_id_map_.find(id);
    if(it != orders_id_map_.end()) {
        auto helper = it->second;
        if(helper.side == BUY) {
            updates.push_back(bids.remove_order(helper));
        } else {
            updates.push_back(asks.remove_order(helper));
        }
        orders_id_map_.erase(id);
    } else {
        LOG_WARN(fmt::format("Could not find order id {}", id));
    }

    for(auto& update: updates) {
        LOG_DEBUG("{}", update.log_level_update());
        if(public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::match_order(Order &order) {
    auto trade_producer = TradeProducer(order, pmr_resource_);
    std::vector<LevelUpdate> updates;
    LOG_DEBUG("{}", trade_producer.log_producer());
    if(order.side() == BUY) {
        updates = asks.match_order(trade_producer);
    } else {
        updates = bids.match_order(trade_producer);
    }

    for(auto& trade: trade_producer.get_trades()) {
        if(private_trades_update_handler) {
            private_trades_update_handler(trade);
        }
        if(public_last_trade_update_handler) {
            public_last_trade_update_handler({trade.price(), trade.qty(), trade.crossing_side()});
        }
    }

    for(auto& update: updates) {
        LOG_DEBUG("{}", update.log_level_update());
        if(public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::add_order_helper(Price price, OrderId order_id, Side side) {
    auto helper = FindOrderHelper(order_id, side, price);
    orders_id_map_.emplace(order_id, std::move(helper));
}
