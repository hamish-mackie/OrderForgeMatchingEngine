#pragma once

#include "book_level.h"
#include "order_book_data.h"

template<typename CompFunc>
class BookSide {
public:
    using LevelsCont = OrderBookDataMap<CompFunc>;
    using AllocateSize = uint64_t;

    BookSide(Side side, Price& tick_size)
        : side_(side)
        , tick_size_(tick_size)
        , levels_(side, tick_size) {}

    LevelUpdate add_order(Order& order);
    LevelUpdate remove_order(FindOrderHelper& helper);
    std::vector<LevelUpdate> match_order(TradeProducer& trade_producer);

    std::optional<Price> best_price() { return levels_.best_price(); }

private:
    Side side_;
    TickSize tick_size_;
    LevelsCont levels_;
};

template<typename CompFunc>
LevelUpdate BookSide<CompFunc>::add_order(Order &order) {
    ZoneScopedN("add");
    LOG_INFO("{}", order.log_order());

    assert(order.side() == side_);

    auto ptr = levels_.get_book_level(order.price());
    return ptr->add_order(order);
}

template<typename CompFunc>
LevelUpdate BookSide<CompFunc>::remove_order(FindOrderHelper& helper) {
    LOG_INFO("{}", helper.order_id);
    auto ptr = levels_.get_book_level(helper.price);
    auto update = ptr->remove_order(helper);
    if(update.total_quantity.value() == 0) {
        levels_.remove_book_level(update.price);
    }
    return update;
}

template<typename CompFunc>
std::vector<LevelUpdate> BookSide<CompFunc>::match_order(TradeProducer& trade_producer) {
    LOG_INFO("{}", trade_producer.log_producer());
    // TODO need to adjust best price to be correct
    // its best price is not being deleted
    std::vector<LevelUpdate> updates;
    for(auto& level: levels_) {
        if(trade_producer.has_remaining_qty()) {
            updates.push_back(level.second->match_order(trade_producer));
        }
    }

    for(auto& update: updates) {
        if(update.total_quantity.value() == 0) {
            levels_.remove_book_level(update.price);
        }
    }

    return updates;
}

