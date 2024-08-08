#include "order_book.h"

int main() {
    auto ob = OrderBook(Price(100), TickSize(0.01));
    ob.private_order_update_handler = [](const Order &order) { };
    ob.private_trades_update_handler = [](const Trade &trade) { };
    ob.public_order_book_update_handler = [](const LevelUpdate &update) { };
    ob.public_last_trade_update_handler = [](const LastTradeUpdate &update) { };

    std::vector<Order> demo_orders = {
        Order(Price(100),  Quantity(10), BUY, OPEN, LIMIT, 000, 1),
        Order(Price(200),  Quantity(10), SELL, OPEN, LIMIT, 000, 2),
        Order(Price(100),  Quantity(10), SELL, OPEN, MARKET, 000, 3),
        Order(Price(200),  Quantity(10), BUY, OPEN, MARKET, 000, 3),
    };

    for(auto& order: demo_orders) {
        ob.add_order(order);
    }

    return 0;
}
