#pragma once

#include <vector>

#include "data_types/order.h"
#include "data_types/trade.h"

class TradeProducer {
public:
    explicit TradeProducer(Order& order);

    std::string log_producer() const;

    bool has_remaining_qty() { return remaining_qty_.value() > 0; }
    Quantity& remaining_qty() { return remaining_qty_; }
    std::vector<Order*>& get_modified_orders_() { return modified_orders_; }
    std::vector<Trade>& get_trades() { return trades_; }

    Quantity match_order(Order& order);

private:
    Order& original_order_;
    Price order_price_;
    Quantity& remaining_qty_;

    // We report a vector of trades made.
    std::vector<Trade> trades_;
    // We also report vector of orders which have been removed or changed.
    std::vector<Order*> modified_orders_;
};

