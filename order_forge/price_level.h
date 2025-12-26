#pragma once

#include "matching_engine.h"
#include "order_forge_types.h"
namespace of {

class PriceLevelUpdate {
public:
    PriceLevelUpdate() : side_() {}

    PriceLevelUpdate(const Symbol symbol, const Price& price, const Quantity& total_quantity, Side side) :
        symbol_(symbol), price_(price), total_qty_(total_quantity), side_(side) {}

    [[nodiscard]] Symbol symbol() const { return symbol_; }
    [[nodiscard]] Quantity total_quantity() const { return total_qty_; }
    [[nodiscard]] Price price() const { return price_; }
    [[nodiscard]] Side side() const { return side_; }

private:
    Symbol symbol_;
    Price price_;
    Quantity total_qty_;
    Side side_;
};

struct PriceLevelUpdateLog {
    Symbol symbol_;
    Price price_;
    Quantity total_qty_;
    Side side_;

    void write(const PriceLevelUpdate& update) {
        symbol_ = update.symbol();
        price_ = update.price();
        total_qty_ = update.total_quantity();
        side_ = update.side();
    }

    std::string get_str() const {
        return fmt::format("Symbol: {}, Price: {}, Total Quantity: {}, Side: {}", symbol_, price_.descaled_value(),
                           total_qty_.descaled_value(), enum_str(side_));
    }
};

class PriceLevel {
public:
    using OrdersCont = HashLinkedList<OrderId, Order>;

    explicit PriceLevel(Symbol symbol, const Price& price, Side side) :
        symbol_(symbol), price_(price), total_qty_(0), side_(side) {}

    PriceLevelUpdate add_order(Order& order);
    PriceLevelUpdate remove_order(FindOrderHelper& helper);
    PriceLevelUpdate remove_order(OrderId id);
    PriceLevelUpdate match_order(MatchingEngine& matching_engine);

    void remove_marked_order_nodes();

    Symbol symbol() const { return symbol_; }
    Quantity total_quantity() const { return total_qty_; }
    size_t size() const { return order_cont.size(); }
    Price price() const { return price_; }

private:
    Symbol symbol_;
    Price price_;
    Quantity total_qty_;
    Side side_;
    OrdersCont order_cont;
    std::vector<OrdersCont::NodeTypePtr> orders_to_be_removed_;
};
} // namespace of
