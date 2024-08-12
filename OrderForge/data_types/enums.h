#pragma once

#include <cstdint>
#include <random>
#include <magic_enum.hpp>

#include "scaled_int.h"

using Price = ScaledInt<int64_t, static_cast<uint64_t>(1e2)>;
using TickSize = Price;
using Quantity = ScaledInt<uint64_t, static_cast<uint64_t>(1e8)>;

using AccountId = uint64_t;

using OrderId = uint64_t;
inline OrderId gen_random_order_id() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
    return dis(gen);
}

enum Side: uint8_t {
    BUY = 0,
    SELL = 1,
};

enum OrderStatus: uint8_t {
    OPEN = 0,
    PARTIAL = 1,
    FILLED = 2,
    CANCELLED = 3,
};

enum OrderType: uint8_t {
    LIMIT = 0,
    MARKET = 1,
};