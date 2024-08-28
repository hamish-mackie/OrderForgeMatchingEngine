#pragma once

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "event_handler_base.h"

class Reactor;

class TCPClientHandler : EventHandler {
public:
    explicit TCPClientHandler(const FileDescriptor fd) : EventHandler(fd) {};
    void handle_event(uint64_t events) override;
    ~TCPClientHandler() override = default;
};

class TCPHandler final : public EventHandler {
public:
    TCPHandler(const FileDescriptor fd, Reactor& reactor) : EventHandler(fd), reactor_(reactor) {}
    void handle_event(uint64_t events) override;
    ~TCPHandler() override = default;

private:
    Reactor& reactor_;
};

