#include "book_level.h"

void BookLevel::add_order(const Order& order) {
    order_cont.push_back(order);
    total_qty_ += order.qty();
}

void BookLevel::add_order(const Order&& order) {
    order_cont.push_back(order);
    total_qty_ += order.qty();
}

void BookLevel::remove_order(OrderId id) {
    auto lambda = [&](const Order& itr) { return itr.order_id() == id; };
    const auto itr = std::ranges::find_if(order_cont, lambda);
    if( itr == order_cont.end()) {
        return;
    }
    total_qty_ -= itr->qty();
    order_cont.erase(itr);
}




