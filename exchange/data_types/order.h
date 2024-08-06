#pragma once

#include "time_utils.h"
#include "enums.h"
#include "price.h"
#include "quantity.h"


class Order {
public:
    Order(const Price price, const Quantity qty, Side side, OrderStatus status, OrderType type, uint64_t acc_id, OrderId order_id)
        : price_(price),
          qty_(qty),
          side_(side),
          status_(status),
          type_(type),
          order_id_(order_id),
          timestamp_(get_nano_ts()),
          acc_id_(acc_id) { }

    [[nodiscard]] const Price & price() const { return price_; }
    [[nodiscard]] const Quantity & qty() const { return qty_; }
    [[nodiscard]] Side side() const { return side_; }
    [[nodiscard]] OrderStatus status() const { return status_; }
    [[nodiscard]] OrderType type() const { return type_; }
    [[nodiscard]] OrderId order_id() const { return order_id_; }
    [[nodiscard]] clock_t timestamp() const { return timestamp_; }
    [[nodiscard]] uint64_t acc_id() const { return acc_id_; }

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
