#pragma once

#include "time_utils.h"
#include "enums.h"
#include "logger/logger.h"

class Order {
public:
    Order(Price price, Quantity qty, Side side, OrderStatus status, OrderType type, uint64_t acc_id, OrderId client_order_id, OrderId order_id = 0);

    [[nodiscard]] Price price() { return price_; }
    [[nodiscard]] Quantity qty() { return qty_; }
    [[nodiscard]] Quantity& remaining_qty_ref() { return remaining_qty_; }
    [[nodiscard]] Quantity remaining_qty() { return remaining_qty_; }
    [[nodiscard]] Side side() const { return side_; }
    [[nodiscard]] OrderStatus status() const { return status_; }
    [[nodiscard]] OrderType type() const { return type_; }
    [[nodiscard]] OrderId client_order_id() const { return client_order_id_; }
    [[nodiscard]] OrderId& client_order_id() { return client_order_id_; }
    [[nodiscard]] OrderId order_id() const { return order_id_; }
    [[nodiscard]] OrderId& order_id() { return order_id_; }
    [[nodiscard]] clock_t timestamp() const { return timestamp_; }
    [[nodiscard]] AccountId acc_id() const { return acc_id_; }

    void set_order_id(const OrderId order_id) {
        assert(order_id != 0);
        order_id_ = order_id;
    }

    void set_status(OrderStatus status) {
        status_ = status;
    }

    Quantity reduce_qty(Quantity& qty);

    void set_qty(Quantity qty) { remaining_qty_ = qty; }

    std::string log_order() const;

private:
    Price price_;
    Quantity qty_;
    Quantity remaining_qty_;
    Side side_;
    OrderStatus status_;
    OrderType type_;
    OrderId client_order_id_;
    OrderId order_id_;
    clock_t timestamp_;
    AccountId acc_id_;
};

inline Order::Order(Price price, Quantity qty, Side side, OrderStatus status, OrderType type, uint64_t acc_id,
                    OrderId client_order_id, OrderId order_id): price_(price), qty_(qty), remaining_qty_(qty), side_(side), status_(status),
                                       type_(type), client_order_id_(client_order_id), order_id_(order_id), timestamp_(get_nano_ts()),
                                       acc_id_(acc_id) {
}

inline Quantity Order::reduce_qty(Quantity &qty) {
    if(qty > remaining_qty_) {
        LOG_ERROR("trying to reduce quantity to below 0", nullptr);
    } else {
        remaining_qty_ -= qty;
    }
    return remaining_qty_;
}

struct OrderLog {
    Price price_;
    Quantity qty_;
    Quantity remaining_qty_;
    Side side_;
    OrderStatus status_;
    OrderType type_;
    OrderId client_order_id_;
    OrderId order_id_;
    clock_t timestamp_;
    AccountId acc_id_;

    void write(Order& order) {
        price_ = order.price();
        qty_ = order.qty();
        remaining_qty_ = order.remaining_qty();
        side_ = order.side();
        status_ = order.status();
        type_ = order.type();
        client_order_id_ = order.client_order_id();
        order_id_ = order.order_id();
        timestamp_ = order.timestamp();
        acc_id_ = order.acc_id();
    }

    std::string get_str() const {
        return fmt::format("Price: {}, Quantity: {} / {}, Side: {}, Status: {}, Type: {}, AccountId: {}, OrderId: {}, ClientOrderId: {}, Timestamp: {}",
                           price_.descaled_value(), remaining_qty_.descaled_value(), qty_.descaled_value(), magic_enum::enum_name(side_),
                           magic_enum::enum_name(status_), magic_enum::enum_name(type_), acc_id_, order_id_, client_order_id_, timestamp_);
    }
};

struct FindOrderHelper {
    FindOrderHelper(OrderId order_id, Side side, const Price &price)
        : order_id(order_id), price(price), side(side) {
    }
    OrderId order_id;
    Price price;
    Side side;
};

