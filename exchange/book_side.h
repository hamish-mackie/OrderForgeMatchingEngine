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

    void add_order(const Order& order);
    void remove_order(OrderId id);
    void match_order(TradeProducer& trade_producer);

private:
    Side side_;
    Price tick_size_;
    LevelsCont levels_;
};

template<typename CompFunc>
void BookSide<CompFunc>::add_order(const Order &order) {
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
    assert(order.side != side_);
}

