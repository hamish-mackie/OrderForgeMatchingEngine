#pragma once

#include <set>
#include <sys/epoll.h>


#include <reactor.h>

#include "tcp_client_handler.h"
#include "tcp_connection_handler.h"


class TCPReactor final : ReactorItem {
public:
    TCPReactor();

    void register_handler(EventHandler* handler, uint32_t events);
    void unregister_handler(EventHandler* handler);

    void broadcast_all(std::string_view message, ConnectionType con_type);
    void broadcast_all(std::string_view message);

    void run() override;

    ~TCPReactor() override;

private:
    static constexpr uint64_t max_events_{100};
    epoll_event events_[max_events_]{};
    FileDescriptor epoll_fd_;

    std::array<std::set<EventHandler*>, UINT8_MAX> event_handlers_;
};
