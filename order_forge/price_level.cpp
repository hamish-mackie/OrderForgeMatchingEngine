#include "price_level.h"
namespace of {

PriceLevelUpdate PriceLevel::add_order(Order& order) {
    LOG_ORDER(order);
    assert(order.order_id() != 0);
    if (order.price() != price_) {
        LOG_WARN("price is not correct, order: {} and member {}", order.price().value(), price_.value());
        assert(0);
    }

    if (order_cont.contains(order.order_id())) {
        LOG_WARN("Order already present in map: {}", order.order_id());
        return {order.symbol(), price_, total_qty_, side_};
    }

    order_cont.push(order.order_id(), order);
    total_qty_ += order.remaining_qty();
    return {symbol(), price_, total_qty_, side_};
}

PriceLevelUpdate PriceLevel::remove_order(FindOrderHelper& helper) { return remove_order(helper.order_id); }

PriceLevelUpdate PriceLevel::remove_order(OrderId id) {
    auto res = order_cont.find(id);
    if (res) {
        LOG_ORDER(*res);
        total_qty_ -= res->remaining_qty();
        order_cont.remove(id);
    } else {
        LOG_WARN("Order not present in map: {}", id);
    }
    return {symbol(), price_, total_qty_, side_};
}

void PriceLevel::remove_marked_order_nodes() {
    for (auto node: orders_to_be_removed_) {
        LOG_ORDER(node->item);
        order_cont.remove_node(node);
    }
    orders_to_be_removed_.clear();
}

PriceLevelUpdate PriceLevel::match_order(MatchingEngine& matching_engine) {
    LOG_DEBUG("{}", matching_engine.log_matching_engine());
    const auto prev_total_qty = total_qty_;

    for (auto &order: order_cont) {
        if (matching_engine.has_remaining_qty()) {
            total_qty_ -= matching_engine.match_order(order.item);
            if (order.item.status() == FILLED) {
                orders_to_be_removed_.push_back(&order);
            }
        }
    }

    if (total_qty_ != prev_total_qty) {
        matching_engine.add_level_to_remove_orders(this);
    }

    return {symbol(), price_, total_qty_, side_};
}
} // namespace of
