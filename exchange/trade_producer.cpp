#include "trade_producer.h"

TradeProducer::TradeProducer(Order &order): original_order_(order)
                                            , order_price_(order.price())
                                            , remaining_qty_(order.remaining_qty_ref()) {
    LOG_INFO("{}", order.log_order());
}

Quantity TradeProducer::match_order(Order &order) {
    if(original_order_.side() == order.side()) {
        LOG_WARN("orders have the same side");
    }

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

    trades_.emplace_back(order.price(), removed_qty, original_order_.side(), order.acc_id(), original_order_.acc_id(),
        order.order_id(), original_order_.order_id());

    LOG_INFO("{}", order.log_order());
    LOG_INFO("{}", original_order_.log_order());
    LOG_INFO("{}", log_producer());
    return removed_qty;
}

std::string TradeProducer::log_producer() const {
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
