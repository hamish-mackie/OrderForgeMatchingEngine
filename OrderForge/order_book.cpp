#include "order_book.h"

OrderBook::OrderBook(std::string symbol, Price starting_price, TickSize tick_size, bool write_std_out) :
    symbol_(std::make_unique<std::string>(symbol)), bids(BookSideBid(BUY, tick_size)),
    asks(BookSideAsk(SELL, tick_size)) {

    Logger::get_instance(write_std_out, MB * 100, 5);
    REGISTER_TYPE(ORDER, Order);
    REGISTER_TYPE(TRADE, Trade);
    REGISTER_TYPE(LEVEL_UPDATE, LevelUpdate);
    REGISTER_TYPE(LAST_TRADE_UPDATE, LastTradeUpdate);
    REGISTER_TYPE(DEBUG, Debug);
    // Call order allocator, so it allocates up front.
    SingleTonWrapper<PoolAllocator<Node<OrderId, Order>>>::get_instance(131072);
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

    if (order.type() == MARKET) {
        match_order(order);
        return;
    }

    if (is_crossing_order(order)) {
        match_order(order);
    }

    // order fully traded
    if (order.remaining_qty().value() == 0) {
        return;
    }

    LevelUpdates updates;

    if (order.side() == BUY) {
        updates.push_back(bids.add_order(order));
        add_order_helper(order.price(), order.order_id(), order.side());
    } else {
        updates.push_back(asks.add_order(order));
        add_order_helper(order.price(), order.order_id(), order.side());
    }

    if (private_order_update_handler) {
        private_order_update_handler(order);
    }

    for (auto &update: updates) {
        LOG_UPDATE_LEVEL(update);
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
        LOG_UPDATE_LEVEL(update);
        if (public_order_book_update_handler) {
            public_order_book_update_handler(update);
        }
    }
}

void OrderBook::match_order(Order &order) {
    auto matching_engine = MatchingEngine(order, pmr_resource_);
    std::vector<LevelUpdate> updates;

    // If there are no orders in the book, reject the market order
    if (order.type() == MARKET) {
        if (order.side() == BUY && asks.empty() || order.side() == SELL && bids.empty()) {
            reject_order(order);
            return;
        }
    }

    LOG_ORDER(order);
    if (order.side() == BUY) {
        updates = asks.match_order(matching_engine);
    } else {
        updates = bids.match_order(matching_engine);
    }

    if (private_order_update_handler) {
        private_order_update_handler(order);
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
    // If there is no orders on the opposite side, nothing to match with. return false.
    if (order.side() == BUY && asks.empty()) {
        return false;
    }
    if (order.side() == SELL && bids.empty()) {
        return false;
    }

    auto opposite_best_price = order.side() == BUY ? asks.best_price() : bids.best_price();

    return (order.side() == BUY && order.price() >= opposite_best_price) ||
           (order.side() == SELL && order.price() <= opposite_best_price);
}
void OrderBook::reject_order(Order &order) {
    order.set_status(REJECTED);
    LOG_ORDER(order);
    if (private_order_update_handler) {
        private_order_update_handler(order);
    }
}

void OrderBook::add_order_helper(Price price, OrderId order_id, Side side) {
    auto helper = FindOrderHelper(order_id, side, price);
    orders_id_map_.emplace(order_id, std::move(helper));
}
