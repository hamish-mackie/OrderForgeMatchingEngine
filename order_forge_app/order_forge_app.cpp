#include "order_forge_app.h"

OrderForgeApp::OrderForgeApp() {
    auto config = read_config_file<OrderForgeAppConfig>();
    Logger::get_instance(config.logger_config);
    Logger::register_common_types();

    LOG_INFO("app id: {}", config.app_id);

    auto* tcp_reactor = new TCPReactor();

    for (const auto [port]: config.listener_configs) {
        auto* tcp_handler = new TCPConnectionHandler(port, *tcp_reactor);
        tcp_handler->add_handler([&](auto buffer, auto size) { handle_event(buffer, size); });
        tcp_reactor->register_handler(tcp_handler, EPOLLIN | EPOLLOUT | EPOLLET);
    };

    for (auto& order_book_config: config.order_book_configs) {
        order_books_.insert({order_book_config.symbol, std::make_unique<OrderBook>(order_book_config)});
    }

    reactor_ = std::make_unique<Reactor>();
    reactor_->add_item(tcp_reactor);
    reactor_->run();
}

void OrderForgeApp::handle_event(const char* buffer, const size_t size) {
    LOG_INFO("{}", std::string_view(buffer, size));

    auto message = std::string_view(buffer, size);
    std::cout << message << std::endl;

    msg::ws::Result<Order> res = msg::ws::parse_order(message);

    if (!res.is_valid()) {
        LOG_ERROR("message not valid: {}", message);
    }

    LOG_ORDER(res.get_value());
}
