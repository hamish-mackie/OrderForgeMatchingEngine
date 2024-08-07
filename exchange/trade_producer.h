#pragma once

#include <vector>
#include <optional>

#include "data_types/order.h"
#include "data_types/trade.h"

// Trade Producer is a class that can be created  the orderbook
// When an order is filled, a trade producer is sent
// A trade producer keeps track of remaining volume in the order
// All trades produced

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

public:
    // std::optional<Order> match_order(Order);

};

