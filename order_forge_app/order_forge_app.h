#pragma once

#include "message_protocol_write_json.h"
#include "message_protocols/json/message_protocol_json.h"
#include "order_book.h"
#include "order_forge_app_config.h"
#include "tcp_reactor.h"

using namespace of;

class OrderForgeApp {
public:
    OrderForgeApp();

    void handle_event(const char* buffer, size_t size);
    void last_trade_update_handler(const LastTradeUpdate& update);
    void order_book_update_handler(const PriceLevelUpdate& update);

private:
    std::unique_ptr<OrderBookConfig> order_book_config_;
    std::unordered_map<Symbol, std::unique_ptr<OrderBook>> order_books_;
    std::unique_ptr<TCPReactor> tcp_reactor_;
    std::unique_ptr<Reactor> reactor_;
};
