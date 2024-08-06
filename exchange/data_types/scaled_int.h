#pragma once

#include <iostream>
#include <cstdint>

template<typename T, uint64_t Scale>
class ScaledInt {
    using int_type = T;
public:

    ScaledInt(): value_(0) {};
    explicit ScaledInt(const double value) : value_(value * Scale) {}
    explicit ScaledInt(int_type value, bool raw) : value_(raw ? value : value * Scale) {}

    void descale() { value_ = value_ / Scale; }

    ScaledInt(const ScaledInt& other): value_(other.value_) {}

    ScaledInt& operator=(const ScaledInt& other) {
        return *this;
    }

    [[nodiscard]] int_type value() const { return value_; }

    [[nodiscard]] double descaled_value() const { return static_cast<double>(value_) / Scale; }

    friend std::ostream& operator<<(std::ostream& os, const ScaledInt& price) {
        os << price.descaled_value();
        return os;
    }

    bool operator==(const ScaledInt &other) const {
        return other.value_ == value_;
    }
    ScaledInt& operator+=(const ScaledInt& other) {
        value_ += other.value_;
        return *this;
    }
    ScaledInt& operator-=(const ScaledInt& other) {
        value_ -= other.value_;
        return *this;
    }
    ScaledInt operator+(const ScaledInt& other) {
        return ScaledInt(value_ + other.value_, true);
    }
    ScaledInt operator-(const ScaledInt& other) {
        return ScaledInt(value_ - other.value_, true);
    }
    ScaledInt operator/(const ScaledInt& other) {
        return ScaledInt(value_ / other.value_);
    }
    ScaledInt operator*(const ScaledInt& other) {
        return ScaledInt(value_ * other.value_ / Scale, true);
    }

private:
    int_type value_;
};