#pragma once

#include <iostream>
#include <cstdint>

// PriceType is by defaut int64_t, because we need to support negative numbers.

template<uint64_t Scale>
class ScaledPrice {
    using PriceType = int64_t;
public:

    ScaledPrice(): value_(0) {};
    explicit ScaledPrice(const double value) : value_(value * Scale) {}

    [[nodiscard]] PriceType price() const { return value_; }

    [[nodiscard]] double descaled_price() const { return static_cast<double>(value_) / Scale; }

    friend std::ostream& operator<<(std::ostream& os, const ScaledPrice& price) {
        os << price.descaled_price();
        return os;
    }

private:
    PriceType value_;
};

using Price = ScaledPrice<static_cast<uint64_t>(1e2)>;
