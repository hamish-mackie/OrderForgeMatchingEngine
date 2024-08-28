#include "event_handler_tcp.h"

#include "event_loop.h"

inline void TCPClientHandler::handle_event(uint64_t events) {
    if (events & EPOLLIN) {
        char buffer[1024];
        ssize_t n = read(fd_, buffer, sizeof(buffer));
        if (n == -1) {
            perror("read");
            close(fd_);
            delete this;
        } else if (n == 0) {
            std::cout << "client disconnected: " << fd_ << std::endl;
            close(fd_);
            delete this;
        } else {
            std::cout << "client received:  " << std::string(buffer, n) << std::endl;
        }
    }
}

inline void TCPHandler::handle_event(uint64_t events) {
    if (events & EPOLLIN) {
        FileDescriptor client_fd = accept(fd_, nullptr, nullptr);
        if (client_fd == -1) {
            perror("accept");
        } else {
            std::cout << "client fd: " << client_fd << std::endl;
            auto* client_handler = new TCPClientHandler(client_fd);
            reactor_.register_handler(reinterpret_cast<EventHandler*>(client_handler), EPOLLIN | EPOLLET);
        }
    }
}