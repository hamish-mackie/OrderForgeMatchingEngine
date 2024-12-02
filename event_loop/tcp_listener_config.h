#pragma once

struct TCPListenerConfig {
    int port{0};

    static TCPListenerConfig from_json(const json& j) {
        TCPListenerConfig config;
        set_config(j, "port", config.port);
        return config;
    }
};
