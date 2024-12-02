#include "tcp_reactor.h"

TCPReactor::TCPReactor() {
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    LOG_INFO("epoll fd: {}", epoll_fd_);
}
void TCPReactor::register_handler(EventHandler* handler, const uint32_t events) {
    LOG_INFO("register fd: {}", handler->get_fd());
    epoll_event ev{};
    ev.data.ptr = handler->get_handle();
    ev.events = events;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->get_fd(), &ev) < 0) {
        if (errno == EEXIST) {
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handler->get_fd(), &ev) < 0) {
                perror("epoll_ctl");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    }
    connected_clients_.insert(handler);
}
void TCPReactor::unregister_handler(EventHandler* handler) {
    LOG_INFO("unregister fd: {}", handler->get_fd());
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->get_fd(), nullptr) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    close(handler->get_fd());
    connected_clients_.erase(handler);
}
void TCPReactor::run() {
    const int n = epoll_wait(epoll_fd_, events_, max_events_, 0);
    if (n == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        auto* handler = static_cast<EventHandler*>(events_[i].data.ptr);
        handler->handle_event(events_[i].events);
    }
}
TCPReactor::~TCPReactor() {
    LOG_DEBUG("Destructor");
    for (auto& handler: connected_clients_) {
        unregister_handler(handler);
    }
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
    }
}
