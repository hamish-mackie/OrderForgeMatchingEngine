#pragma once

#include "event_handler_base.h"

class TCPReactor;

class TCPClientHandler : EventHandler {
public:
    TCPClientHandler(const FileDescriptor fd, TCPReactor& reactor,
                     std::vector<std::function<void(const char*, size_t)>>& handlers) :
        EventHandler(fd), reactor_(reactor), handlers_(handlers){};
    void handle_event(uint64_t events) override;
    ~TCPClientHandler() override = default;

private:
    TCPReactor& reactor_;
    std::vector<std::function<void(const char*, size_t)>>& handlers_;
};
