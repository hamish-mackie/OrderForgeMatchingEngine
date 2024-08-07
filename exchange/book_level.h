#pragma once

#include "order.h"
#include "trade_producer.h"

struct LevelUpdate {
    Price price;
    Quantity total_quantity;

    std::string log_level_update() {
        return fmt::format("Price: {}, TotalQty: {}", price.descaled_value(), total_quantity.descaled_value());
    }
};

class BookLevel {
public:
    using OrdersCont = std::vector<Order>; // This is probably better with a ringbuffer and map look up for removals

    explicit BookLevel(const Price& price): price_(price), total_qty_(0) {}
    LevelUpdate add_order(Order& order);
    LevelUpdate remove_order(FindOrderHelper& helper);
    LevelUpdate remove_order(OrderId id);
    void match_order(TradeProducer& trade_producer);

    Quantity total_quantity() { return total_qty_; }
    size_t size() const { return order_cont.size(); }
    Price price() { return price_; }

private:
    Price price_;
    Quantity total_qty_;
    OrdersCont order_cont;
};
