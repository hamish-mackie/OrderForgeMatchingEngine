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
            close(fd_);
            reactor_.unregister_handler(this);
            delete this;
        } else {
            auto message = std::string(buffer, n);
            LOG_INFO("client fd({}) received: {}", fd_, message);
            for (const auto& handler: handlers_) {
                handler(buffer, n);
            }
            if (write(fd_, message.c_str(), strlen(message.c_str())) < 0) {
                perror("write");
            }
        }
    }
}
