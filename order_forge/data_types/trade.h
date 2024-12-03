#pragma once

#include "enums.h"
#include "types.h"
namespace of {

class LastTradeUpdate {
public:
    LastTradeUpdate(const Symbol symbol, const Price &price, const Quantity &quantity, Side side) :
        symbol_(symbol), price_(price), quantity_(quantity), side_(side) {}

    [[nodiscard]] Symbol symbol() const { return symbol_; }
    [[nodiscard]] const Price &price() const { return price_; }
    [[nodiscard]] const Quantity &quantity() const { return quantity_; }
    [[nodiscard]] Side side() const { return side_; }

    Symbol symbol_;
    Price price_;
    Quantity quantity_;
    Side side_;
};

struct LastTradeUpdateLog {
    Symbol symbol_;
    Price price_;
    Quantity quantity_;
    Side side_;

    void write(const LastTradeUpdate &update) {
        symbol_ = update.symbol_;
        price_ = update.price();
        quantity_ = update.quantity();
        side_ = update.side();
    }

    std::string get_str() const {
        return fmt::format("Symbol: {}, Price: {}, Quantity: {}, Side: {}", symbol_, price_.descaled_value(), quantity_.descaled_value(),
                           enum_str(side_));
    }
};

class Trade {
public:
    Trade(Symbol symbol, const Price &price, const Quantity &qty, Side crossing_side, AccountId passive_account_id,
          AccountId crossing_account_id, OrderId passive_order_id, OrderId crossing_order_id) :
        symbol_(symbol), price_(price), qty_(qty), crossing_side_(crossing_side),
        passive_account_id_(passive_account_id), crossing_account_id_(crossing_account_id),
        passive_order_id_(passive_order_id), crossing_order_id_(crossing_order_id), trade_id_(get_id<Trade>()) {}

    [[nodiscard]] Symbol symbol() const { return symbol_; }
    [[nodiscard]] const Price &price() const { return price_; }
    [[nodiscard]] const Quantity &qty() const { return qty_; }
    [[nodiscard]] Side crossing_side() const { return crossing_side_; }
    [[nodiscard]] AccountId passive_account_id() const { return passive_account_id_; }
    [[nodiscard]] AccountId crossing_account_id() const { return crossing_account_id_; }
    [[nodiscard]] OrderId passive_order_id() const { return passive_order_id_; }
    [[nodiscard]] OrderId crossing_order_id() const { return crossing_order_id_; }
    [[nodiscard]] TradeId trade_id() const { return trade_id_; }

private:
    Symbol symbol_;
    Price price_;
    Quantity qty_;
    Side crossing_side_;
    AccountId passive_account_id_;
    AccountId crossing_account_id_;
    OrderId passive_order_id_;
    OrderId crossing_order_id_;
    TradeId trade_id_;
};

struct TradeLog {
    void write(Trade &trade) {
        symbol_ = trade.symbol();
        price_ = trade.price();
        qty_ = trade.qty();
        crossing_side_ = trade.crossing_side();
        passive_account_id_ = trade.passive_account_id();
        crossing_account_id_ = trade.crossing_account_id();
        passive_order_id_ = trade.passive_order_id();
        crossing_order_id_ = trade.crossing_order_id();
        trade_id_ = trade.trade_id();
    }

    Symbol symbol_;
    Price price_;
    Quantity qty_;
    Side crossing_side_;
    AccountId passive_account_id_;
    AccountId crossing_account_id_;
    OrderId passive_order_id_;
    OrderId crossing_order_id_;
    TradeId trade_id_;

    std::string get_str() const {
        return fmt::format("Trade: Symbol: {}, Price: {}, Quantity: {}, Crossing Side: {}, Passive Account ID: {}, "
                           "Crossing Account ID: {}, "
                           "Passive Order ID: {}, Crossing Order ID: {}, TradeId: {}",
                           symbol_, price_.descaled_value(), qty_.descaled_value(), enum_str(crossing_side_),
                           passive_account_id_, crossing_account_id_, passive_order_id_, crossing_order_id_, trade_id_);
    }
};
} // namespace of
