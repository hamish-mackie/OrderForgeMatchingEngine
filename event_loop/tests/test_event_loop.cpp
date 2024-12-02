#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "reactor.h"

TEST(TestEventLoop, gen_test) {

    Reactor reactor;

    // Set up TCP server
    // int tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // sockaddr_in tcp_address = {AF_INET, htons(8080), INADDR_ANY};
    // bind(tcp_server_fd, (sockaddr*) &tcp_address, sizeof(tcp_address));
    // listen(tcp_server_fd, 10);
    //
    // TCPHandler tcp_handler(tcp_server_fd, reactor);
    // reactor.register_handler(&tcp_handler, EPOLLIN);

    // // Set up WebSocket server
    // int ws_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // sockaddr_in ws_address = {AF_INET, htons(8081), INADDR_ANY};
    // bind(ws_server_fd, (sockaddr*) &ws_address, sizeof(ws_address));
    // listen(ws_server_fd, 10);
    //
    // WebSocketHandler ws_handler(ws_server_fd, reactor);
    // reactor.register_handler(&ws_handler, EPOLLIN);

    // Start event loop
    // reactor.run();
}
