#pragma once

#include <unordered_map>

#include "order.h"

class BookLevel {
public:
    using OrdersCont = std::vector<Order>;

    explicit BookLevel(const Price price): price_(price), total_qty_(0) {}
    void add_order(const Order& order);
    void add_order(const Order&& order);
    void remove_order(OrderId id);

    void validate_total_qty() const;

    void fill_order(Order& order);

    Quantity total_quantity() { return total_qty_; }
    size_t size() { return order_cont.size(); }

private:
    Price price_;
    Quantity total_qty_;
    OrdersCont order_cont;
    std::unordered_map<OrderId, Order*> orders_map;
};
