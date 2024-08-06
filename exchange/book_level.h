#pragma once

#include "order.h"
#include "trade_producer.h"

class BookLevel {
public:
    using OrdersCont = std::vector<Order>; // This is probably better with a ringbuffer and map look up for removals

    BookLevel(const Price& price): price_(price), total_qty_(0) {}
    void add_order(const Order& order);
    void remove_order(OrderId id);
    void match_order(TradeProducer& trade_producer);

    Quantity total_quantity() { return total_qty_; }
    size_t size() { return order_cont.size(); }
    Price price() { return price_; }

private:
    Price price_;
    Quantity total_qty_;
    OrdersCont order_cont;
};
