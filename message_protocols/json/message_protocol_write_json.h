#pragma once

#include "order_forge/price_level.h"

using json = nlohmann::json;
using namespace of;

static constexpr auto ORDER_KEY = "o";
static constexpr auto TRADE_KEY = "t";
static constexpr auto LAST_TRADE_KEY = "l";
static constexpr auto PRICE_LEVEL_UPDATE_KEY = "p";

namespace msg::ws {

inline std::string to_json_string(const Order& order) {
    const json json_obj = {
        {"event", ORDER_KEY},
        {"symbol", order.symbol()},
        {"data", {
            {"symbol", order.symbol()},
            {"price", order.price().descaled_value()},
            {"quantity", order.qty().descaled_value()},
            {"remaining_quantity", order.remaining_qty().descaled_value()},
            {"side", magic_enum::enum_name<Side>(order.side())},
            {"status", magic_enum::enum_name<OrderStatus>(order.status())},
            {"type", magic_enum::enum_name<OrderType>(order.type())},
            {"client_order_id", order.client_order_id()},
            {"order_id", order.order_id()},
            {"timestamp", order.timestamp()},
            {"account_id", order.acc_id()}
        }}
    };

    return json_obj.dump() + "\n";
}

inline std::string to_json_string(const Trade& trade) {
    const json json_obj = {
        {"event", TRADE_KEY},
        {"symbol", trade.symbol()},
        {"data", {
            {"symbol", trade.symbol()},
            {"price", trade.price().descaled_value()},
            {"quantity", trade.qty().descaled_value()},
            {"crossing_side", magic_enum::enum_name<Side>(trade.crossing_side())},
            {"passive_account_id", trade.passive_account_id()},
            {"crossing_account_id", trade.crossing_account_id()},
            {"passive_order_id", trade.passive_order_id()},
            {"crossing_order_id", trade.crossing_order_id()},
            {"trade_id", trade.trade_id()}
        }}
    };

    return json_obj.dump() + "\n";
}

inline std::string to_json_string(const PriceLevelUpdate& update) {
    const json json_obj = {
        {"event", PRICE_LEVEL_UPDATE_KEY},
        {"symbol", update.symbol()},
        {"data", {
            {"price", update.price().descaled_value()},
            {"total_quantity", update.total_quantity().descaled_value()},
            {"side", magic_enum::enum_name<Side>(update.side())}
        }}
    };

    return json_obj.dump() + "\n";
}

inline std::string to_json_string(const LastTradeUpdate& update) {
    const json json_obj = {
        {"event", LAST_TRADE_KEY},
        {"symbol:", update.symbol()},
        {"data", {
            {"price", update.price().descaled_value()},
            {"quantity", update.quantity().descaled_value()},
            {"side", magic_enum::enum_name<Side>(update.side())}
        }}
    };

    return json_obj.dump() + "\n";
}

} // namespace msg::ws
