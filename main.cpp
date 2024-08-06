#include <iostream>

#include "order_book.h"

int main() {
    std::cout << "hello world!" << std::endl;
    auto starting_price = Price(100);
    auto tick_size = Price(100);
    OrderBook ob(starting_price, tick_size);
    ob.account_update_handler = []() { std::cout << "account update" << std::endl; };
    ob.trades_update_handler = []() { std::cout << "account update" << std::endl; };
    ob.order_book_update_handler = []() { std::cout << "account update" << std::endl; };
    ob.last_trade_update_handler = []() { std::cout << "account update" << std::endl; };

    auto order = Order(Price(100), Quantity(1), BUY, OPEN, LIMIT, 9999, gen_random_order_id());
    ob.add_order(order);
    return 0;
}
