#pragma once

// Quantity is a uint64_t because it should never be lower than zero.

template<uint64_t Scale>
class ScaledQuantity {
    using QuantityType = uint64_t;
public:
    ScaledQuantity(): value_(0) {}
    explicit ScaledQuantity(const double value) : value_(static_cast<uint64_t>(value * Scale)) {}

    [[nodiscard]] QuantityType quantity() const { return value_; }

    [[nodiscard]] double descaled_quantity() const { return static_cast<double>(value_) / Scale; }

    friend std::ostream& operator<<(std::ostream& os, const ScaledQuantity& price) {
        os << price.descaled_quantity();
        return os;
    }

    bool operator==(const ScaledQuantity& other) const {
        return value_ == other.value_;
    }

    ScaledQuantity& operator+=(const ScaledQuantity& other) {
        value_ += other.value_;
        return *this;
    }
    ScaledQuantity& operator-=(const ScaledQuantity& other) {
        value_ -= other.value_;
        return *this;
    }

private:
    QuantityType value_;
};

using Quantity = ScaledQuantity<static_cast<uint64_t>(1e8)>;
