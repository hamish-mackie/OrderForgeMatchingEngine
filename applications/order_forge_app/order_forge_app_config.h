#pragma once

#include "config/config_helpers.h"

#include "logger_config.h"
#include "tcp_listener_config.h"

struct OrderForgeAppConfig {
    std::string app_id = {"no id"};
    LoggerConfig logger_config;
    std::vector<TCPListenerConfig> listener_configs;
    std::vector<of::OrderBookConfig> order_book_configs;

    static OrderForgeAppConfig from_json(const json& j) {
        OrderForgeAppConfig config;
        set_config(j, "app_id", config.app_id);

        config.logger_config = set_config<LoggerConfig>(j, "logger_config");
        config.listener_configs = set_configs<TCPListenerConfig>(j, "tcp_listeners");
        config.order_book_configs = set_configs<OrderBookConfig>(j, "order_book_configs");

        return config;
    }
};
