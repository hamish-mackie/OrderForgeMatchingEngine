#pragma once

#include "event_handler_base.h"

class TCPReactor;

// Accepts a tcp connection and creates a TCPClientHandler
class TCPConnectionHandler final : public EventHandler {
public:
    TCPConnectionHandler(uint32_t port, ConnectionType next_con_type, TCPReactor& reactor);
    void handle_event(uint64_t events) override;
    void send_buffer(const char*, size_t) override { LOG_ERROR("no impl"); };
    void send_buffer(std::string_view message) override { LOG_ERROR("no impl"); };
    ~TCPConnectionHandler() override = default;

    void add_handler(const std::function<void(const char*, size_t)>& handler) { handlers_.push_back(handler); }

private:
    TCPReactor& reactor_;
    // This is the connection type of the connection once accepted.
    // The connection type of the handler is set as CONNECTION_HANDLER
    ConnectionType next_connection_type_;
    std::vector<std::function<void(const char*, size_t)>> handlers_;
};
