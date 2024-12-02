#include "tcp_client_handler.h"

#include "tcp_reactor.h"

inline void TCPClientHandler::handle_event(uint64_t events) {
    if (events & EPOLLIN) {
        char buffer[1024];
        ssize_t n = read(fd_, buffer, sizeof(buffer));
        if (n == -1) {
            perror("read");
            close(fd_);
            delete this;
        } else if (n == 0) {
            LOG_INFO("client disconnected: {}", fd_);
            reactor_.unregister_handler(this);
            delete this;
        } else {
            auto message = std::string(buffer, n);
            LOG_INFO("client fd({}) received: {}", fd_, message);
            for (const auto& handler: handlers_) {
                handler(buffer, n);
            }

            send(buffer, n);
        }
    }
}

void TCPClientHandler::send(const char* buffer, const size_t size) {
    if (!write(fd_, buffer, size)) {
        perror("write");
    }
}

void TCPClientHandler::send(const std::string_view message) {
    if (!write(fd_, message.data(), message.size())) {
        perror("write");
    }
}
