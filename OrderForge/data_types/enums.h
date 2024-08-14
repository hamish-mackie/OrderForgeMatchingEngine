#pragma once

#include <cstdint>
#include <random>

#include "types.h"

inline OrderId gen_random_order_id() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
    return dis(gen);
}

enum Side : uint8_t {
    BUY = 0,
    SELL = 1,
};

enum OrderStatus : uint8_t {
    OPEN = 0,
    PARTIAL = 1,
    FILLED = 2,
    CANCELLED = 3,
};

enum OrderType : uint8_t {
    LIMIT = 0,
    MARKET = 1,
};
