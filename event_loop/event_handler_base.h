#pragma once

#include <cstdint>
#include <defines.h>
#include <logger.h>
#include <netinet/in.h>

using FileDescriptor = int32_t;

enum class ConnectionType: uint8_t {
    UNDEFINED = 0,
    CONNECTION_HANDLER = 1,

    PRIVATE_FEED_ORDERS = 10,
    PRIVATE_FEED_TRADES = 11,
    PRIVATE_FEED_ACCOUNT = 12,

    PUBLIC_FEED_ORDER_BOOK = 20,
    PUBLIC_FEED_LAST_TRADE = 21,
};

class EventHandler {
public:
    EventHandler() : fd_(0), connection_type_(ConnectionType::UNDEFINED) {}
    explicit EventHandler(const FileDescriptor fd, const ConnectionType con_type) : fd_(fd), connection_type_(con_type) {}

    virtual void handle_event(uint64_t events) = 0;
    virtual void send_buffer(const char*, size_t) = 0;
    virtual void send_buffer(std::string_view message) = 0;

    void* get_handle() { return static_cast<void*>(this); }
    [[nodiscard]] FileDescriptor get_fd() const { return fd_; }
    [[nodiscard]] ConnectionType get_connection_type() const { return connection_type_; }

    virtual ~EventHandler() = default;

protected:
    FileDescriptor fd_;
    ConnectionType connection_type_;
};
