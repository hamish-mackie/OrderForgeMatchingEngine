#include "tcp_connection_handler.h"

#include "tcp_reactor.h"

TCPConnectionHandler::TCPConnectionHandler(const uint32_t port, TCPReactor& reactor) : reactor_(reactor) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in tcp_address{};
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(port);
    tcp_address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(fd_);
        exit(EXIT_FAILURE);
    }

    if (bind(fd_, reinterpret_cast<sockaddr*>(&tcp_address), sizeof(tcp_address)) < 0) {
        perror("failed to bind fd");
        close(fd_);
        exit(EXIT_FAILURE);
    }

    if (listen(fd_, 10) < 0) {
        perror("failed to listen on socket");
        close(fd_);
        exit(EXIT_FAILURE);
    }

    LOG_INFO("listening fd({}) on port({}) for tcp connection", fd_, port);
}

inline void TCPConnectionHandler::handle_event(const uint64_t events) {
    if (events & EPOLLIN) {
        FileDescriptor client_fd = accept(fd_, nullptr, nullptr);
        if (client_fd == -1) {
            perror("accept");
        } else {
            LOG_INFO("client connected: {}", fd_);
            auto* client_handler = new TCPClientHandler(client_fd, reactor_, handlers_);
            reactor_.register_client_handler(reinterpret_cast<EventHandler*>(client_handler),
                                             EPOLLIN | EPOLLOUT | EPOLLET);
        }
    }
}
