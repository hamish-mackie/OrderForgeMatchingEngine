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

    void add_order(Order& order);
    void remove_order(OrderId id);
    void match_order(TradeProducer& trade_producer);

    std::optional<Price> best_price() { return levels_.best_price(); }

private:
    Side side_;
    Price tick_size_;
    LevelsCont levels_;
};

template<typename CompFunc>
void BookSide<CompFunc>::add_order(Order &order) {
    LOG_INFO(magic_enum::enum_name(side_), order.log_order());

    assert(order.side() == side_);

    auto ptr = levels_.get_book_level(order.price());
    ptr->add_order(order);
    // receive a limit order
    // check best price,
    // if the order is a buy and the price of the order is higher than best price,
    // we should fill the order
}

template<typename CompFunc>
void BookSide<CompFunc>::remove_order(OrderId id) {

}

template<typename CompFunc>
void BookSide<CompFunc>::match_order(TradeProducer& trade_producer) {
    LOG_INFO(magic_enum::enum_name(side_), trade_producer.log_producer());

    for(auto& level: levels_) {
        if(trade_producer.has_remaining_qty()) {
            level.second->match_order(trade_producer);
        }
    }
}

