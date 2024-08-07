#include "book_level.h"

#include <order_book.h>

LevelUpdate BookLevel::add_order(Order& order) {
    LOG_INFO(price_.descaled_value(), order.log_order());
    if(order.price() != price_) { LOG_WARN("price is not correct"); }

    order_cont.push_back(order);
    total_qty_ += order.qty();
    return {price_, total_qty_};
}

LevelUpdate BookLevel::remove_order(FindOrderHelper& helper) {
    return remove_order(helper.order_id);
}

LevelUpdate BookLevel::remove_order(OrderId id) {
    LOG_INFO(id);
    auto lambda = [&](const Order& itr) { return itr.order_id() == id; };
    const auto itr = std::ranges::find_if(order_cont, lambda);
    if( itr == order_cont.end()) {
        LOG_WARN(fmt::format("could not find order {}", id));
        return {price_, total_qty_};
    }
    total_qty_ -= itr->qty();
    order_cont.erase(itr);
    return {price_, total_qty_};
}

void BookLevel::match_order(TradeProducer &trade_producer) {
    LOG_INFO(price_.descaled_value(), trade_producer.log_producer());

    for(auto& order: order_cont) {
        if(trade_producer.has_remaining_qty())
            trade_producer.match_order(order);
    }

    for(const auto* order: trade_producer.get_modified_orders_()) {
        if(order->status() == FILLED) {
            remove_order(order->order_id());
        }
    }

}




