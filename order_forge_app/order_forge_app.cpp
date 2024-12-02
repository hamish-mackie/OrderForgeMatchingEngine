#include "order_forge_app.h"

OrderForgeApp::OrderForgeApp() {
    auto config = read_config_file<OrderForgeAppConfig>();
    Logger::get_instance(config.logger_config);
    Logger::register_common_types();

    LOG_INFO("app id: {}", config.app_id);

    tcp_reactor_ = std::make_unique<TCPReactor>();

    for (const auto [port]: config.listener_configs) {
        auto* tcp_handler = new TCPConnectionHandler(port, *tcp_reactor_);
        tcp_handler->add_handler([&](auto buffer, auto size) { handle_event(buffer, size); });
        tcp_reactor_->register_connection_handler(tcp_handler, EPOLLIN | EPOLLOUT | EPOLLET);
    };

    for (auto& order_book_config: config.order_book_configs) {
        auto res = order_books_.insert({order_book_config.symbol, std::make_unique<OrderBook>(order_book_config)});
        if (!res.second) {
            LOG_ERROR("Failed to create order book: {}", order_book_config.symbol);
        }

        res.first->second->public_order_book_update_handler = [&](auto update) { order_book_update_handler(update); };
        res.first->second->public_last_trade_update_handler = [&](auto update) { last_trade_update_handler(update); };
    }

    reactor_ = std::make_unique<Reactor>();
    reactor_->add_item(tcp_reactor_.get());
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

    Order& order = res.get_value();

    if (auto res = order_books_.find(order.symbol()); res != order_books_.end()) {
        order.set_symbol(res->first);
        res->second->add_order(order);

    } else {
        LOG_WARN("symbol not found: {}", order.symbol());
    }
}

void OrderForgeApp::last_trade_update_handler(const LastTradeUpdate& update) {
    const std::string update_string = msg::ws::to_json_string(update);
    tcp_reactor_->broadcast_all(update_string);
}

void OrderForgeApp::order_book_update_handler(const PriceLevelUpdate& update) {
    const std::string update_string = msg::ws::to_json_string(update);
    tcp_reactor_->broadcast_all(update_string);
}
