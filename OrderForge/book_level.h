#pragma once

#include "order_forge_types.h"
#include "matching_engine.h"

class LevelUpdate {
public:
    LevelUpdate(const Price &price, const Quantity &total_quantity, Side side)
        : price_(price),
          total_qty_(total_quantity),
          side_(side) {
    }

    [[nodiscard]] Quantity total_quantity() const { return total_qty_; }
    [[nodiscard]] Price price() const { return price_; }
    [[nodiscard]] Side side() const { return side_; }

private:
    Price price_;
    Quantity total_qty_;
    Side side_;
};

class BookLevel {
public:
    using OrdersCont = HashLinkedList<OrderId, Order>;

    explicit BookLevel(const Price& price, Side side): price_(price), total_qty_(0), side_(side) {}
    LevelUpdate add_order(Order& order);
    LevelUpdate remove_order(FindOrderHelper& helper);
    LevelUpdate remove_order(OrderId id);
    LevelUpdate match_order(MatchingEngine& matching_engine);

    void remove_node();

    Quantity total_quantity() const { return total_qty_; }
    size_t size() const { return order_cont.size(); }
    Price price() const { return price_; }

private:
    Price price_;
    Quantity total_qty_;
    Side side_;
    OrdersCont order_cont;
    std::vector<Node<uint64_t, Order>*> orders_to_be_removed_;
};
