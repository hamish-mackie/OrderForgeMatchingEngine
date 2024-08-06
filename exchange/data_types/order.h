#pragma once

#include "time_utils.h"
#include "enums.h"
#include "logger/logger.h"

class Order {
public:
    Order(Price price, Quantity qty, Side side, OrderStatus status, OrderType type, uint64_t acc_id, OrderId order_id)
        : price_(price),
          qty_(qty),
          side_(side),
          status_(status),
          type_(type),
          order_id_(order_id),
          timestamp_(get_nano_ts()),
          acc_id_(acc_id) { }

    [[nodiscard]] const Price price() const { return price_; }
    [[nodiscard]] const Quantity qty() const { return qty_; }
    [[nodiscard]] Side side() const { return side_; }
    [[nodiscard]] OrderStatus status() const { return status_; }
    [[nodiscard]] OrderType type() const { return type_; }
    [[nodiscard]] OrderId order_id() const { return order_id_; }
    [[nodiscard]] clock_t timestamp() const { return timestamp_; }
    [[nodiscard]] uint64_t acc_id() const { return acc_id_; }

    std::string log_order() const {
        return fmt::format("Price: {}, Quantity: {}, Side: {}, Status: {}, Type: {}, AccountId: {}, OrderId: {}, Timestamp: {}",
                  price_.descaled_value(), qty_.descaled_value(), magic_enum::enum_name(side_),
                  magic_enum::enum_name(status_), magic_enum::enum_name(type_), acc_id_, order_id_, timestamp_);
    }

private:
    Price price_;
    Quantity qty_;
    Side side_;
    OrderStatus status_;
    OrderType type_;
    OrderId order_id_;
    clock_t timestamp_;
    uint64_t acc_id_;
};
