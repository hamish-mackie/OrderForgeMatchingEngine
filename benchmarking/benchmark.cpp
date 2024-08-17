#include <algorithm>
#include <chrono>
#include <random>

#include <tracy/Tracy.hpp>
#include "order_book.h"
namespace of {

static constexpr std::string_view symbol = "TESTUSD";

double generate_price(std::default_random_engine& generator, std::normal_distribution<double>& distribution) {
    double price;
    do {
        price = distribution(generator);
    } while (price < 90.0 || price > 110.0);
    return price;
}

OrderBook generate_order_book() {
    Price start_price = Price(100);
    TickSize tick_size = TickSize(0.01);
    return OrderBook(symbol.data(), start_price, tick_size);
}

std::vector<Order> generate_orders(int num_orders) {
    std::vector<Order> orders;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(100.0, 3.0);

    int market_order_count = num_orders * 0.1;
    int limit_order_count = num_orders - market_order_count;

    for (int i = 0; i < limit_order_count / 2; ++i) {
        double price = generate_price(generator, distribution);
        if (price > 99.99)
            price = 99.99;
        orders.emplace_back(symbol, Price(price), Quantity(1), BUY, OPEN, LIMIT, i);
    }

    for (int i = 0; i < limit_order_count / 2; ++i) {
        double price = generate_price(generator, distribution);
        if (price < 100)
            price = 100;
        orders.emplace_back(symbol, Price(price), Quantity(1), SELL, OPEN, LIMIT, limit_order_count / 2 + i,
                            limit_order_count / 2 + i);
    }

    for (int i = 0; i < market_order_count / 2; ++i) {
        orders.emplace_back(symbol, Price(110), Quantity(1), BUY, OPEN, MARKET, limit_order_count + i,
                            limit_order_count + i);
    }
    for (int i = 0; i < market_order_count / 2; ++i) {
        orders.emplace_back(symbol, Price(90), Quantity(1), SELL, OPEN, MARKET,
                            limit_order_count + market_order_count / 2 + i,
                            limit_order_count + market_order_count / 2 + i);
    }

    // Shuffle the orders to mix buys, sells, and market orders
    std::shuffle(orders.begin(), orders.end(), generator);

    return orders;
}

std::chrono::duration<double> benchmark_market_orders(OrderBook&& ob, std::vector<Order>& orders) {
    auto start = std::chrono::high_resolution_clock::now();

    for (auto& order: orders) {
        ob.add_order(order);
    }

    for (const auto& order: orders) {
        ob.remove_order(order.order_id());
    }

    auto end = std::chrono::high_resolution_clock::now();
    return end - start;
}

void benchmark_order_book(uint64_t num_orders) {
    auto orders = generate_orders(num_orders);

    auto duration_market_orders = benchmark_market_orders(generate_order_book(), orders);

    double market_ops_per_sec = num_orders / duration_market_orders.count();

    fmt::print("Time to insert and remove {} orders with market orders crossing the book: {:.6f} seconds {} "
               "orders/second)\n",
               fmt::group_digits(orders.size()), duration_market_orders.count(), fmt::group_digits(market_ops_per_sec));
}

// void *operator new(std ::size_t count) {
//     auto ptr = malloc(count);
//     TracyAlloc(ptr, count);
//     return ptr;
// }
// void operator delete(void *ptr) noexcept {
//     TracyFree(ptr);
//     free(ptr);
// }

} // namespace of

using namespace of;


int main() {
    TracyAppInfo("Benchmark", 0);

    auto log_cfg = LoggerConfig{};
    log_cfg.write_std_out = false;
    log_cfg.mem_block_size = of::MB * 100;
    log_cfg.number_blocks = 2;
    // Call order allocator, so it allocates up front.
    Logger::get_instance(log_cfg);
    SingleTonWrapper<PoolAllocator<Node<OrderId, Order>>>::get_instance().expand_pool(of::MB * 50);
    std::vector<u_int64_t> num_orders = {50000, 100000, 500000};
    for (auto& n: num_orders) {
        benchmark_order_book(n);
    }
    Logger::get_instance().stop();
    return 0;
}
