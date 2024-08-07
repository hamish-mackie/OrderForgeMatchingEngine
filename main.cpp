#include <iostream>

#include "order_book.h"

int main() {
    std::cout << "hello world!" << std::endl;
    auto starting_price = Price(100);
    auto tick_size = Price(0.1);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() { std::cout << "account update" << std::endl; };
    ob.trades_update_handler = [](const Trade& trade) {
        std::cout << trade.log_trade() << std::endl;
    };
    ob.order_book_update_handler = []() { std::cout << "account update" << std::endl; };
    ob.last_trade_update_handler = []() { std::cout << "account update" << std::endl; };

    std::vector<Order> limit_orders = {
        Order(Price(99.3), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id()),
        Order(Price(99.5), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id()),
        Order(Price(99.7), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id()),
        Order(Price(100.2), Quantity(1), SELL, OPEN, LIMIT, 9999, gen_random_order_id()),
        Order(Price(100.4), Quantity(1), SELL, OPEN, LIMIT, 9999, gen_random_order_id()),
        Order(Price(100.6), Quantity(1), SELL, OPEN, LIMIT, 9999, gen_random_order_id())
    };
    for (auto& order : limit_orders) {
        ob.add_order(order);
    }

    std::vector<Order> market_orders = {
        Order(Price(99.5), Quantity(2), SELL, OPEN, MARKET, 9999, gen_random_order_id()),
        Order(Price(100.4), Quantity(2), BUY, OPEN, MARKET, 9999, gen_random_order_id())
    };

    for (auto& order : market_orders) {
        ob.add_order(order);
    }

    return 0;
}
