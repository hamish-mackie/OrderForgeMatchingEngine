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
    explicit TradeProducer(Order& order)
        : original_order_(order)
        , order_price_(order.price())
        , remaining_qty_(order.qty()) {
        LOG_INFO(order.log_order());
    }
    std::string log_producer() const {
        return fmt::format("Trade Producer: Price: {}, Quantity: {} / {}, Side: {}, Status: {}, Type: {}, AccountId: {}, OrderId: {}, Timestamp: {}",
                  original_order_.price().descaled_value(),
                  remaining_qty_.descaled_value(),
                  original_order_.qty().descaled_value(),
                  magic_enum::enum_name(original_order_.side()),
                  magic_enum::enum_name(original_order_.status()),
                  magic_enum::enum_name(original_order_.type()),
                  original_order_.acc_id(),
                  original_order_.order_id(),
                  original_order_.timestamp());
    }

    bool has_remaining_qty() { return remaining_qty_.value() > 0; }
    Quantity& remaining_qty() { return remaining_qty_; }
    std::vector<Order*>& get_modified_orders_() { return modified_orders_; }
    std::vector<Trade>& get_trades() { return trades_; }

    void match_order(Order& order) {
        if(original_order_.side() == order.side()) {
            LOG_WARN("orders have the same side");
        }

        if(order.remaining_qty() == remaining_qty_) {
            trades_.emplace_back(order.price(), order.remaining_qty(), original_order_.side(),
                order.order_id(), original_order_.order_id(),
                order.acc_id(), original_order_.acc_id());
            LOG_INFO(trades_.back().log_trade());
            order.reduce_qty(remaining_qty_);
            order.set_status(FILLED);
            remaining_qty_.set_value(0);
            modified_orders_.push_back(&order);
        }

        else if (order.remaining_qty() > remaining_qty_) {
            order.reduce_qty(remaining_qty_);
            order.set_status(PARTIAL);
            remaining_qty_.set_value(0);
            modified_orders_.push_back(&order);

        } else if (order.remaining_qty() < remaining_qty_) {
            auto q = order.remaining_qty();
            remaining_qty_ -= order.remaining_qty();
            order.reduce_qty(q);
            order.set_status(FILLED);
            modified_orders_.push_back(&order);
        }

        if(remaining_qty_.value() == 0) {original_order_.set_status(FILLED); }
        else if(remaining_qty_.value() > 0 && remaining_qty_ < original_order_.qty()) { original_order_.set_status(PARTIAL); }

        LOG_INFO("matched", order.log_order());
        LOG_INFO(log_producer());
    }

private:
    Order& original_order_;
    Price order_price_;
    Quantity remaining_qty_;

    // We report a vector of trades made.
    std::vector<Trade> trades_;
    // We also report vector of orders which have been removed or changed.
    std::vector<Order*> modified_orders_;

public:
    // std::optional<Order> match_order(Order);

};

