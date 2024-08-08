#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include "order_book.h"

// Function to generate a price closer to 100
double generate_price(std::default_random_engine &generator, std::normal_distribution<double> &distribution) {
    double price;
    do {
        price = distribution(generator);
    } while (price < 90.0 || price > 110.0);
    return price;
}

void benchmark_order_book() {
    Price start_price = Price(100);
    Price tick_size = Price(0.01);
    OrderBook ob(start_price, tick_size);


    int num_orders = 100000;
    std::vector<Order> orders;

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(100.0, 3.0); // Normal distribution centered at 100

    // Generate buy orders with prices between 90-99.99
    for (int i = 0; i < num_orders / 2; ++i) {
        double price = generate_price(generator, distribution);
        if (price > 99.99) price = 99.99; // Cap buy prices at 99.99
        orders.emplace_back(Price(price), Quantity(1), BUY, OPEN, LIMIT, i, i);
    }

    // Generate sell orders with prices between 100-110
    for (int i = 0; i < num_orders / 2; ++i) {
        double price = generate_price(generator, distribution);
        if (price < 100) price = 100; // Floor sell prices at 100
        orders.emplace_back(Price(price), Quantity(1), SELL, OPEN, LIMIT, num_orders / 2 + i, num_orders / 2 + i);
    }

    // Shuffle the orders to mix buys and sells
    std::shuffle(orders.begin(), orders.end(), generator);

    // Benchmark inserting and removing 100,000 orders
    auto start = std::chrono::high_resolution_clock::now();

    for (auto &order : orders) {
        ob.add_order(order);
    }

    for (auto &order : orders) {
        ob.remove_order(order.order_id());
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time to insert and remove 100,000 orders: " << duration.count() << " seconds" << std::endl;

    // Benchmark inserting and removing orders with market orders crossing the book
    start = std::chrono::high_resolution_clock::now();

    for (auto &order : orders) {
        ob.add_order(order);
    }

    // Add some market orders that cross the book
    for (int i = 0; i < 10; ++i) {
        Order market_buy(Price(110), Quantity(10), BUY, OPEN, MARKET, num_orders + i, num_orders + i);
        Order market_sell(Price(90), Quantity(10), SELL, OPEN, MARKET, num_orders + i + 10, num_orders + i + 10);
        ob.add_order(market_buy);
        ob.add_order(market_sell);
    }

    for (auto &order : orders) {
        ob.remove_order(order.order_id());
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time to insert and remove orders with market orders crossing the book: " << duration.count() << " seconds" << std::endl;
}

int main() {
    benchmark_order_book();
    return 0;
}
