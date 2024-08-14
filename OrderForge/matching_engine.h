#pragma once

#include <memory_resource>

#include "order_forge_types.h"

class BookLevel;

class MatchingEngine {
public:
    explicit MatchingEngine(Order &order, std::pmr::unsynchronized_pool_resource &vec_resource);

    std::string log_matching_engine() const;

    bool has_remaining_qty() { return remaining_qty_.value() > 0; }
    Quantity &remaining_qty() { return remaining_qty_; }
    std::pmr::vector<Order *> &get_modified_orders_() { return modified_orders_; }
    std::pmr::vector<Trade> &get_trades() { return trades_; }

    Quantity match_order(Order &order);
    void add_level_to_remove_orders(BookLevel *book_level) { remove_levels.push_back(book_level); }
    std::pmr::vector<BookLevel *>& get_levels_to_remove_orders() { return remove_levels; }

private:
    Order &original_order_;
    Price order_price_;
    Quantity &remaining_qty_;

    // We report a vector of trades made.
    std::pmr::vector<Trade> trades_;
    // We also report vector of orders which have been removed or changed.
    std::pmr::vector<Order *> modified_orders_;
    std::pmr::vector<BookLevel *> remove_levels;
    std::pmr::unsynchronized_pool_resource &vec_resource_;
};
