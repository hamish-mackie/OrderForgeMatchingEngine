#include "book_level.h"

void BookLevel::add_order(Order& order) {
    if(order.price() != price_) {
        LOG_WARN("price is not correct", ' ');
    }

    LOG_INFO(price_.descaled_value(), order.log_order());
    order_cont.push_back(order);
    total_qty_ += order.qty();
}

void BookLevel::remove_order(OrderId id) {
    LOG_INFO(id);
    auto lambda = [&](const Order& itr) { return itr.order_id() == id; };
    const auto itr = std::ranges::find_if(order_cont, lambda);
    if( itr == order_cont.end()) {
        return;
    }
    total_qty_ -= itr->qty();
    order_cont.erase(itr);
}

void BookLevel::match_order(TradeProducer &trade_producer) {
    LOG_INFO(price_.descaled_value(), trade_producer.log_producer());

    for(auto& order: order_cont) {
        if(trade_producer.has_remaining_qty())
            trade_producer.match_order(order);
    }

    auto orders = trade_producer.get_orders();

    for(auto order: orders) {
        if(order->status() == FILLED) {
            remove_order(order->order_id());
        }
    }

}




