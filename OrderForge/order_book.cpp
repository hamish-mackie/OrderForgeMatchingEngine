#include "order_book.h"

OrderBook::OrderBook(std::string symbol, Price starting_price, TickSize tick_size, bool write_std_out) :
    symbol_(std::make_unique<std::string>(symbol)), bids(BookSideBid(BUY, tick_size)),
    asks(BookSideAsk(SELL, tick_size)) {

    Logger::get_instance(write_std_out, MB * 50, 5);
    REGISTER_TYPE(ORDER, Order);
    REGISTER_TYPE(TRADE, Trade);
    REGISTER_TYPE(DEBUG, Debug);
    // Call order allocator, so it allocates up front.
    SingleTonWrapper<PoolAllocator<Node<OrderId, Order>>>::get_instance(131072);
}

void OrderBook::add_order(Order &order) {
    LevelUpdates updates;

    if (order.symbol() != *symbol_) {
        LOG_WARN("Symbol {} does not match {}", order.symbol(), symbol_->data());
        return;
    }

    // set symbol to orderbooks symbol, so it guarantees its lifetime
    order.set_symbol(*symbol_);

    // if order id is not provided, set one.
    if (!order.order_id()) {
        order.set_order_id();
    }

    if (order.type() == MARKET) {
        match_order(order);
    }

    if (order.type() == LIMIT) {
        if (order.side() == BUY) {
            auto best_price = asks.best_price();
            if (best_price.has_value() && best_price <= order.price()) {
                match_order(order);
            }
            if (order.remaining_qty().value() > 0) {
                updates.push_back(bids.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
                if (private_order_update_handler) {
                    private_order_update_handler(order);
                }
            }
        } else {
            auto best_price = bids.best_price();
            if (best_price.has_value() && best_price >= order.price()) {
                match_order(order);
            }
            if (order.remaining_qty().value() > 0) {
                updates.push_back(asks.add_order(order));
                add_order_helper(order.price(), order.order_id(), order.side());
                if (private_order_update_handler) {
                    private_order_update_handler(order);
                }
            }
        }
    }

    for (auto &update: updates) {
        // TODO Log Type Correctly
        // LOG_DEBUG("{}", update.log_level_update());
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::remove_order(OrderId id) {
    LOG_DEBUG("{}", id);
    LevelUpdates updates;
    auto it = orders_id_map_.find(id);
    if (it != orders_id_map_.end()) {
        auto helper = it->second;
        if (helper.side == BUY) {
            updates.push_back(bids.remove_order(helper));
        } else {
            updates.push_back(asks.remove_order(helper));
        }
        orders_id_map_.erase(id);
    } else {
        LOG_WARN(fmt::format("Could not find order id {}", id));
    }

    for (auto &update: updates) {
        // LOG_DEBUG("{}", update.log_level_update());
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::match_order(Order &order) {
    auto matching_engine = MatchingEngine(order, pmr_resource_);
    std::vector<LevelUpdate> updates;
    LOG_DEBUG("{}", matching_engine.log_matching_engine());
    if (order.side() == BUY) {
        updates = asks.match_order(matching_engine);
    } else {
        updates = bids.match_order(matching_engine);
    }
    LOG_ORDER(order);

    for (auto modified_order: matching_engine.get_modified_orders_()) {
        if (private_order_update_handler) {
            private_order_update_handler(*modified_order);
        }
    }

    for (const auto &level: matching_engine.get_levels_to_remove_orders()) {
        level->remove_marked_order_nodes();
    }

    for (auto &trade: matching_engine.get_trades()) {
        if (private_trades_update_handler) {
            private_trades_update_handler(trade);
        }
        if (public_last_trade_update_handler) {
            public_last_trade_update_handler({trade.price(), trade.qty(), trade.crossing_side()});
        }
    }

    for (auto &update: updates) {
        // LOG_DEBUG("{}", update.log_level_update());
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::add_order_helper(Price price, OrderId order_id, Side side) {
    auto helper = FindOrderHelper(order_id, side, price);
    orders_id_map_.emplace(order_id, std::move(helper));
}
