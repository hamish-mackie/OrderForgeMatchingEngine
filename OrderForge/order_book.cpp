#include "order_book.h"

OrderBook::OrderBook(std::string symbol, Price starting_price, TickSize tick_size, bool write_std_out) :
    symbol_(std::make_unique<std::string>(symbol)), bids(BookSideBid(BUY, tick_size)),
    asks(BookSideAsk(SELL, tick_size)) {

    Logger::get_instance(write_std_out, MB * 100, 2);
    REGISTER_TYPE(ORDER, Order);
    REGISTER_TYPE(TRADE, Trade);
    REGISTER_TYPE(LEVEL_UPDATE, LevelUpdate);
    REGISTER_TYPE(LAST_TRADE_UPDATE, LastTradeUpdate);
    REGISTER_TYPE(DEBUG, Debug);
    // Call order allocator, so it allocates up front.
    SingleTonWrapper<PoolAllocator<Node<OrderId, Order>>>::get_instance().expand_pool(MB * 50);
}

void OrderBook::add_order(Order &order) {
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

    switch (order.type()) {
        case MARKET: {
            market_order(order);
            break;
        }
        case FILL_AND_KILL: {
            fill_and_kill_order(order);
            break;
        }
        case LIMIT: {
            limit_order(order);
            break;
        }
    }

    if (private_order_update_handler) {
        private_order_update_handler(order);
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
        LOG_UPDATE_LEVEL(update);
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::limit_order(Order &order) {
    if (is_crossing_order(order)) {
        match_order(order);
    }
    if (order.status() == FILLED) {
        return;
    }

    LevelUpdate update;

    if (order.is_buy()) {
        update = bids.add_order(order);
        add_order_helper(order.price(), order.order_id(), order.side());
    } else {
        update = asks.add_order(order);
        add_order_helper(order.price(), order.order_id(), order.side());
    }

    LOG_UPDATE_LEVEL(update);
    if (public_order_book_update_handler) {
        public_order_book_update_handler(update);
    }
}

void OrderBook::market_order(Order &order) {
    if (order.is_buy() && asks.empty() || !order.is_buy() && bids.empty()) {
        reject_order(order);
        return;
    }

    match_order(order);
}

void OrderBook::fill_and_kill_order(Order &order) {
    if (is_crossing_order(order)) {
        match_order(order);
    } else {
        reject_order(order);
    }
}

void OrderBook::match_order(Order &order) {
    auto matching_engine = MatchingEngine(order, pmr_resource_);
    std::vector<LevelUpdate> updates;

    LOG_ORDER(order);
    if (order.is_buy()) {
        updates = asks.match_order(matching_engine);
    } else {
        updates = bids.match_order(matching_engine);
    }

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
            auto last_trade_update = LastTradeUpdate(trade.price(), trade.qty(), trade.crossing_side());
            public_last_trade_update_handler(last_trade_update);
        }
    }

    for (auto &update: updates) {
        LOG_UPDATE_LEVEL(update);
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

bool OrderBook::is_crossing_order(Order &order) {
    bool is_buy = order.is_buy();
    if ((is_buy && asks.empty()) || (!is_buy && bids.empty())) {
        return false;
    }
    auto opposite_best_price = is_buy ? asks.best_price() : bids.best_price();
    return is_buy ? (order.price() >= opposite_best_price) : (order.price() <= opposite_best_price);
}

void OrderBook::reject_order(Order &order) {
    order.set_status(REJECTED);
    LOG_ORDER(order);
}

void OrderBook::add_order_helper(Price price, OrderId order_id, Side side) {
    auto helper = FindOrderHelper(order_id, side, price);
    orders_id_map_.emplace(order_id, std::move(helper));
}
