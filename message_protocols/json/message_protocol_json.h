#pragma once

#include "enums.h"
#include "nlohmann/json.hpp"
#include "order.h"

using json = nlohmann::json;
using namespace of;

// JSON KEYS
static constexpr auto SYMBOL_KEY = "symbol";
static constexpr auto PRICE_KEY = "price";
static constexpr auto QTY_KEY = "qty";
static constexpr auto ORDER_SIDE_KEY = "order_side";
static constexpr auto ORDER_TYPE_KEY = "order_type";
static constexpr auto ORDER_STATUS_KEY = "order_status";
static constexpr auto ACC_ID_KEY = "acc_id";
static constexpr auto CLIENT_ORDER_ID_KEY = "client_order_id";
static constexpr auto ORDER_ID_KEY = "order_id";

namespace msg::ws {

constexpr size_t MAX_ERROR_CODE = 10000;

constexpr std::array<std::string_view, MAX_ERROR_CODE> initialize_error_messages() {
    std::array<std::string_view, MAX_ERROR_CODE> errors = {};
    for (size_t i = 0; i < MAX_ERROR_CODE; ++i) {
        errors[i] = "unset error code"; // Default error message
    }

    errors[51] = "missing field: symbol";
    errors[52] = "missing field: price";
    errors[53] = "missing field: qty";
    errors[54] = "missing field: order_side";
    errors[55] = "missing field: order_type";
    errors[56] = "missing field: order_status";
    errors[57] = "missing field: acc_id";
    errors[58] = "missing field: client_order_id";
    errors[59] = "missing field: order_id";

    errors[120] = "invalid order_side value";
    errors[121] = "invalid order_type value";
    errors[122] = "invalid order_status value";
    return errors;
}

static constexpr std::array<std::string_view, MAX_ERROR_CODE> errors_arr = initialize_error_messages();


template<typename T>
    requires(!std::same_as<T, std::string_view>)
class Result {
private:
    // TODO this copys data, it should be fixed to be a ref or something lighter
    std::variant<T, std::string_view> data;

public:
    explicit Result(const T& value) : data(value) {}

    explicit Result(const std::string_view error) : data(error) {}

    bool is_valid() const { return std::holds_alternative<T>(data); }

    const T& get_value() const {
        if (!is_valid()) {
            throw std::runtime_error("result does not contain a valid object.");
        }
        return std::get<T>(data);
    }

    T& get_value() {
        if (!is_valid()) {
            throw std::runtime_error("result does not contain a valid object.");
        }
        return std::get<T>(data);
    }

    const std::string_view get_error() const {
        if (is_valid()) {
            throw std::runtime_error("result does not contain an error message.");
        }
        return std::get<std::string_view>(data);
    }
};

template<typename T>
std::string_view get_error_message();

template<>
inline std::string_view get_error_message<Side>() {
    return errors_arr[120];
}
template<>
inline std::string_view get_error_message<OrderType>() {
    return errors_arr[121];
}
template<>
inline std::string_view get_error_message<OrderStatus>() {
    return errors_arr[122];
}

template<typename EnumType>
Result<EnumType> parse_enum(const json& j, const std::string& field_name) {
    std::string value = j.at(field_name);
    std::optional<EnumType> opt_enum = magic_enum::enum_cast<EnumType>(value, magic_enum::case_insensitive);
    if (!opt_enum.has_value()) {
        return Result<EnumType>(get_error_message<EnumType>());
    }
    return Result(opt_enum.value());
}

inline std::optional<std::string_view> validate_required_fields(const json& j) {
    if (!j.contains(SYMBOL_KEY)) {
        return errors_arr[51];
    }
    if (!j.contains(PRICE_KEY)) {
        return errors_arr[52];
    }
    if (!j.contains(QTY_KEY)) {
        return errors_arr[53];
    }
    if (!j.contains(ORDER_SIDE_KEY)) {
        return errors_arr[54];
    }
    if (!j.contains(ORDER_TYPE_KEY)) {
        return errors_arr[55];
    }
    if (!j.contains(ORDER_STATUS_KEY)) {
        return errors_arr[56];
    }
    if (!j.contains(ACC_ID_KEY)) {
        return errors_arr[57];
    }
    if (!j.contains(CLIENT_ORDER_ID_KEY)) {
        return errors_arr[58];
    }
    if (!j.contains(ORDER_ID_KEY)) {
        return errors_arr[59];
    }
    return {};
}

static Result<Order> parse_order(std::string_view message) {
    json j = json::parse(message);

    if (auto res = validate_required_fields(j); res.has_value()) {
        return Result<Order>(res.value());
    }

    Symbol symbol = j.at(SYMBOL_KEY).get<Symbol>();
    if (symbol.empty()) {
        return Result<Order>(errors_arr[51]);
    }

    Price price = Price(j.at(PRICE_KEY).get<double>());
    Quantity qty = Quantity(j.at(QTY_KEY).get<double>());

    Side side;
    if (const auto res = parse_enum<Side>(j, ORDER_SIDE_KEY); res.is_valid()) {
        side = res.get_value();
    } else {
        return Result<Order>(res.get_error());
    }

    OrderType type;
    if (const auto res = parse_enum<OrderType>(j, ORDER_TYPE_KEY); res.is_valid()) {
        type = res.get_value();
    } else {
        return Result<Order>(res.get_error());
    }

    OrderStatus status;
    if (const auto res = parse_enum<OrderStatus>(j, ORDER_STATUS_KEY); res.is_valid()) {
        status = res.get_value();
    } else {
        return Result<Order>(res.get_error());
    }

    uint64_t acc_id = j.at(ACC_ID_KEY).get<uint64_t>();
    OrderId client_order_id = j.at(CLIENT_ORDER_ID_KEY).get<OrderId>();
    OrderId order_id = j.at(ORDER_ID_KEY).get<OrderId>();

    of::Order order(symbol, price, qty, side, status, type, acc_id, client_order_id, order_id);

    return Result<Order>(order);
}

} // namespace msg::ws
