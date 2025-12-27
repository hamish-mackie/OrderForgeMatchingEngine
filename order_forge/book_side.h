#pragma once

#include "order_book_data.h"
#include "price_level.h"

namespace of {

template<typename CompFunc>
class BookSide {
public:
    using LevelsCont = OrderBookDataMap<CompFunc>;
    using AllocateSize = uint64_t;

    BookSide(Symbol symbol, Side side, Price& tick_size) :
        side_(side), tick_size_(tick_size), levels_(symbol, side, tick_size) {}

    PriceLevelUpdate add_order(Order& order);
    PriceLevelUpdate remove_order(FindOrderHelper& helper);
    PriceLevelUpdate remove_orders(Price price, std::vector<OrderId>& order_ids);
    std::vector<PriceLevelUpdate> match_order(MatchingEngine& matching_engine);

    Price best_price() { return levels_.best_price(); }
    bool empty() { return levels_.empty(); }

private:
    Symbol symbol_;
    Side side_;
    TickSize tick_size_;
    LevelsCont levels_;
};

template<typename CompFunc>
PriceLevelUpdate BookSide<CompFunc>::add_order(Order& order) {
    assert(order.side() == side_);

    auto ptr = levels_.get_book_level(order.price());
    return ptr->add_order(order);
}

template<typename CompFunc>
PriceLevelUpdate BookSide<CompFunc>::remove_order(FindOrderHelper& helper) {
    auto ptr = levels_.get_book_level(helper.price);
    auto update = ptr->remove_order(helper);
    if (update.total_quantity().value() == 0) {
        levels_.remove_book_level(update.price());
    }
    return update;
}

template<typename CompFunc>
PriceLevelUpdate BookSide<CompFunc>::remove_orders(Price price, std::vector<OrderId>& order_ids) {
    auto ptr = levels_.get_book_level(price);
    auto update = ptr->remove_order(price);
    if (update.total_quantity().value() == 0) {
        levels_.remove_book_level(update.price());
    }
    return update;
}


template<typename CompFunc>
std::vector<PriceLevelUpdate> BookSide<CompFunc>::match_order(MatchingEngine& matching_engine) {
    std::vector<PriceLevelUpdate> updates;
    for (auto&& [price, price_level]: levels_) {
        if (matching_engine.has_remaining_qty()) {
            updates.push_back(price_level->match_order(matching_engine));
        }
    }

    for (auto& update: updates) {
        if (update.total_quantity().value() == 0) {
            levels_.remove_book_level(update.price());
        }
    }

    return updates;
}
} // namespace of
