#pragma once

#include "config_helpers.h"

namespace of {

struct OrderBookConfig {
    std::string symbol{"test_symbol"};
    TickSize tick_size{0.1};

    static OrderBookConfig from_json(const json& j) {
        OrderBookConfig config;
        set_config(j, "symbol", config.symbol);
        double temp_tick_size = 0;
        set_config(j, "tick_size", temp_tick_size);
        config.tick_size = TickSize(temp_tick_size);
        return config;
    }
};

} // namespace of
