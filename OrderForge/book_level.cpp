#include "book_level.h"

LevelUpdate BookLevel::add_order(Order& order) {
    LOG_ORDER(order);
    assert(order.order_id() != 0);
    if(order.price() != price_) {
        LOG_WARN("price is not correct, order: {} and member {}", order.price().value(), price_.value());
        assert(0);
    }

    if(order_cont.contains(order.order_id())) {
        LOG_WARN("Order already present in map: {}", order.order_id());
        return {price_, total_qty_, side_};
    }

    order_cont.push(order.order_id(), order);
    total_qty_ += order.remaining_qty();
    return {price_, total_qty_, side_};
}

LevelUpdate BookLevel::remove_order(FindOrderHelper& helper) {
    return remove_order(helper.order_id);
}

LevelUpdate BookLevel::remove_order(OrderId id) {
    auto res = order_cont.find(id);
    if(res) {
        LOG_ORDER(*res);
        total_qty_ -= res->remaining_qty();
        order_cont.remove(id);
    } else {
        LOG_WARN("Order not present in map: {}", id);
    }
    return {price_, total_qty_, side_};
}

LevelUpdate BookLevel::match_order(MatchingEngine &matching_engine) {
    LOG_DEBUG("{}", matching_engine.log_matching_engine());

    for(auto& order: order_cont) {
        if(matching_engine.has_remaining_qty()) {
            total_qty_ -= matching_engine.match_order(order.item);
        }
    }

    for(auto* order: matching_engine.get_modified_orders_()) {
        if(order->status() == FILLED && price_ == order->price()) {
            remove_order(order->order_id());
        }
    }
    return {price_, total_qty_, side_};
}




