#include "order_book.h"

void order_handler(const Order& order) {
    fmt::println("Order: Price {}, Quantity {}/{}, Side: {}, Status: {}", order.price(), order.remaining_qty(), order.qty(), enum_str(order.side()), enum_str(order.status()));
}

void trade_handler(const Trade& trade) {
    fmt::println("Trade: Price {}, Quantity {}, Side: {}", trade.price(), trade.qty(), enum_str(trade.crossing_side()));
}

void level_update_handler(const LevelUpdate& update) {
    fmt::println("Level Update: Price: {}, Quantity: {}, Side: {}", update.price(), update.total_quantity(), enum_str(update.side()));
}

void last_trade_handler(const LastTradeUpdate& update) {
    fmt::println("Last Trade: Price: {}, Quantity: {}, Side: {}", update.price(), update.quantity(), enum_str(update.side()));
}

int main() {
    std::string_view symbol = "TESTUSD";
    auto ob = OrderBook(symbol.data(), Price(100), TickSize(0.01), true);
    ob.private_order_update_handler = order_handler;
    ob.private_trades_update_handler = trade_handler;
    ob.public_order_book_update_handler = level_update_handler;
    ob.public_last_trade_update_handler = last_trade_handler;

    std::vector<Order> demo_orders = {
        Order(symbol, Price(100),  Quantity(10), BUY, OPEN, LIMIT, 000, 1),
        Order(symbol, Price(100),  Quantity(10), SELL, OPEN, MARKET, 000, 2),
        Order(symbol, Price(200),  Quantity(10), SELL, OPEN, LIMIT, 000, 3),
        Order(symbol, Price(200),  Quantity(10), BUY, OPEN, MARKET, 000, 4),
    };

    for(auto& order: demo_orders) {
        ob.add_order(order);
        fmt::println(" ------ ");
    }

    return 0;
}
