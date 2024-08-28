#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "event_handler_tcp.h"

class Reactor {
public:
    Reactor() {
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
    }

    void register_handler(EventHandler* handler, const uint32_t events) const {
        epoll_event ev{};
        ev.data.ptr = handler->get_handle();
        ev.events = events;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->get_fd(), &ev) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    }

    void unregister_handler(EventHandler* handler) const {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->get_fd(), nullptr) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    }

    void run() const {
        constexpr uint64_t max_events = 100;
        epoll_event events[max_events];

        while (true) {
            int n = epoll_wait(epoll_fd_, events, max_events, -1);
            if (n == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < n; i++) {
                auto* handler = static_cast<EventHandler*>(events[i].data.ptr);
                handler->handle_event(events[i].events);
            }
        }
    }

    ~Reactor() {
        if (epoll_fd_ != -1) {
            close(epoll_fd_);
        }
    }

private:
    FileDescriptor epoll_fd_;
    // EventHandler* handler_;
};
