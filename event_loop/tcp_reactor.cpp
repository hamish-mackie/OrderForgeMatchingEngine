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
    LOG_INFO("register fd: {}, con_type: {}", handler->get_fd(), magic_enum::enum_name(handler->get_connection_type()));
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

    ConnectionType ct = handler->get_connection_type();
    event_handlers_[static_cast<uint8_t>(ct)].insert(handler);
}

void TCPReactor::unregister_handler(EventHandler* handler) {
    LOG_INFO("unregister fd: {}", handler->get_fd());
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->get_fd(), nullptr) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    close(handler->get_fd());
    ConnectionType ct = handler->get_connection_type();
    event_handlers_[static_cast<uint8_t>(ct)].erase(handler);
}

void TCPReactor::broadcast_all(const std::string_view message, ConnectionType con_type) {

    auto is_con_type = [con_type](const EventHandler* handler) {
        return handler->get_connection_type() == con_type;
    };

    for (const auto& set: event_handlers_) {
        for(const auto handler: set | std::views::filter(is_con_type)) {
            handler->send_buffer(message);
        }
    }
}

void TCPReactor::broadcast_all(const std::string_view message) {
    auto is_connected_client = [](const EventHandler* handler) {
        if (handler->get_connection_type() == ConnectionType::UNDEFINED ||
            handler->get_connection_type() == ConnectionType::CONNECTION_HANDLER) {
            return false;
        }
        return true;
    };

    for (const auto& set: event_handlers_) {
        for(const auto handler: set | std::views::filter(is_connected_client)) {
            handler->send_buffer(message);
        }
    }
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
    for (const auto& set: event_handlers_) {
        for(const auto handler: set) {
            unregister_handler(handler);
        }
    }
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
    }
}
