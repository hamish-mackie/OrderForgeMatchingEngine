#pragma once
#include <event_handler_base.h>

struct TCPListenerConfig {
    int port{0};
    ConnectionType connection_type;

    static TCPListenerConfig from_json(const json& j) {
        TCPListenerConfig config;
        set_config(j, "port", config.port);
        parse_enum_config(j, "type", config.connection_type);
        return config;
    }
};
