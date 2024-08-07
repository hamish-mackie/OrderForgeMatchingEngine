#pragma once

#include "enums.h"
#include "logger/logger.h"

class Trade {
public:
    Trade(const Price &price, const Quantity &qty, Side crossing_side, AccountId passive_account_id,
        AccountId crossing_account_id, OrderId passive_order_id, OrderId crossing_order_id)
        : price_(price),
          qty_(qty),
          crossing_side_(crossing_side),
          passive_account_id_(passive_account_id),
          crossing_account_id_(crossing_account_id),
          passive_order_id_(passive_order_id),
          crossing_order_id_(crossing_order_id) {}

    std::string log_trade() const {
        return fmt::format("Trade: Price: {}, Quantity: {}, Crossing Side: {}, Passive Account ID: {}, Crossing Account ID: {}, "
                           "Passive Order ID: {}, Crossing Order ID: {}",
                           price_.descaled_value(), qty_.descaled_value(), magic_enum::enum_name(crossing_side_),
                           passive_account_id_, crossing_account_id_, passive_order_id_, crossing_order_id_);
    }

private:
    Price price_;
    Quantity qty_;
    // The crossing side of the trade
    Side crossing_side_;
    AccountId passive_account_id_;
    AccountId crossing_account_id_;
    OrderId passive_order_id_;
    OrderId crossing_order_id_;
};
