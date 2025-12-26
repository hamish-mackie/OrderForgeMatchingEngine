#pragma once

#include "event_handler_base.h"

class TCPReactor;

class TCPClientHandler : EventHandler {
public:
    TCPClientHandler(const FileDescriptor fd, const ConnectionType con_type, TCPReactor& reactor,
                     std::vector<std::function<void(const char*, size_t)>>& handlers) :
        EventHandler(fd, con_type), reactor_(reactor), handlers_(handlers) {};
    void handle_event(uint64_t events) override;
    void send_buffer(const char*, size_t) override;
    void send_buffer(std::string_view message) override;
    ~TCPClientHandler() override = default;

private:
    TCPReactor& reactor_;
    std::vector<std::function<void(const char*, size_t)>>& handlers_;
};
