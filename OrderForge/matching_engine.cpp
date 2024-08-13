#include "matching_engine.h"

MatchingEngine::MatchingEngine(Order &order, std::pmr::unsynchronized_pool_resource& vec_resource)
    : original_order_(order)
    , order_price_(order.price())
    , remaining_qty_(order.remaining_qty_ref())
    , vec_resource_(vec_resource)

    {}

Quantity MatchingEngine::match_order(Order &order) {
    if(original_order_.side() == order.side()) {
        LOG_WARN("orders have the same side");
    }

    LOG_ORDER(original_order_);
    LOG_ORDER(order);

    Quantity removed_qty = std::min(order.remaining_qty(), remaining_qty_);
    order.reduce_qty(removed_qty);
    remaining_qty_ -= removed_qty;

    if(order.remaining_qty() == remaining_qty_) {
        order.set_status(FILLED);
        original_order_.set_status(FILLED);
    }
    else if (order.remaining_qty() > remaining_qty_) {
        order.set_status(PARTIAL);
        original_order_.set_status(FILLED);
    } else if (order.remaining_qty() < remaining_qty_) {
        order.set_status(FILLED);
        original_order_.set_status(PARTIAL);
    }

    modified_orders_.push_back(&order);

    trades_.emplace_back(order.symbol(), order.price(), removed_qty, original_order_.side(), order.acc_id(), original_order_.acc_id(),
        order.order_id(), original_order_.order_id());

    LOG_TRADE(trades_.back());

    return removed_qty;
}

std::string MatchingEngine::log_matching_engine() const {
    return fmt::format("Matching Engine: Symbol: {}, Price: {}, Quantity: {} / {}, Side: {}, Status: {}, Type: {}, AccountId: {}, OrderId: {}, Timestamp: {}",
                       original_order_.symbol(),
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

