#pragma once

#include "order_forge/price_level.h"

using json = nlohmann::json;
using namespace of;

namespace msg::ws {

inline std::string to_json_string(const Order& order) {
  return "order\n";
}

inline std::string to_json_string(const Trade& trade) {
  return "trade\n";
}

inline std::string to_json_string(const PriceLevelUpdate& update) {
    json json_obj = {{"price", update.price().descaled_value()},
                     {"total_quantity", update.total_quantity().descaled_value()},
                     {"side", magic_enum::enum_name<Side>(update.side())}};

    return json_obj.dump() + "\n";
}

inline std::string to_json_string(const LastTradeUpdate& update) {
    const json json_obj = {{"price", update.price().descaled_value()},
                           {"quantity", update.quantity().descaled_value()},
                           {"side", magic_enum::enum_name<Side>(update.side())}};

    return json_obj.dump() + "\n";
}
} // namespace msg::ws
