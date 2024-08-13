#pragma once

#include "logger/logger.h"
#include "order.h"
#include "matching_engine.h"
#include "hashed_linked_list.h"

struct LevelUpdate {
    LevelUpdate(const Price &price, const Quantity &total_quantity, Side side)
        : price(price),
          total_quantity(total_quantity),
          side(side) {
    }

    Price price;
    Quantity total_quantity;
    Side side;

    std::string log_level_update() {
        return fmt::format("Level Update: Price: {}, TotalQty: {}, Side: {}",
            price.descaled_value(), total_quantity.descaled_value(), magic_enum::enum_name(side));
    }
};

class BookLevel {
public:
    using OrdersCont = HashLinkedList<OrderId, Order>;

    explicit BookLevel(const Price& price, Side side): price_(price), total_qty_(0), side_(side) {}
    LevelUpdate add_order(Order& order);
    LevelUpdate remove_order(FindOrderHelper& helper);
    LevelUpdate remove_order(OrderId id);
    LevelUpdate match_order(MatchingEngine& matching_engine);

    Quantity total_quantity() { return total_qty_; }
    size_t size() const { return order_cont.size(); }
    Price price() { return price_; }

private:
    Price price_;
    Quantity total_qty_;
    Side side_;
    OrdersCont order_cont;
};
