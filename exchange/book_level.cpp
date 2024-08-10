#include "book_level.h"

LevelUpdate BookLevel::add_order(Order& order) {
    if(order.price() != price_) { LOG_WARN("price is not correct"); }

    if(order_cont.contains(order.order_id())) {
        LOG_WARN("Order already present in map: {}", order.log_order());
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
        LOG_DEBUG("{}", res->log_order());
        total_qty_ -= res->remaining_qty();
        order_cont.remove(id);
    } else {
        LOG_WARN("Order not present in map: {}", id);
    }
    return {price_, total_qty_, side_};
}

LevelUpdate BookLevel::match_order(TradeProducer &trade_producer) {
    LOG_DEBUG("{}", trade_producer.log_producer());

    for(auto& order: order_cont) {
        if(trade_producer.has_remaining_qty()) {
            total_qty_ -= trade_producer.match_order(order.item);
        }
    }

    for(auto* order: trade_producer.get_modified_orders_()) {
        if(order->status() == FILLED && price_ == order->price()) {
            remove_order(order->order_id());
        }
    }
    return {price_, total_qty_, side_};
}




