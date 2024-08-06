#pragma once

#include <vector>
#include <optional>

#include "data_types/order.h"
#include "data_types/trade.h"

// Trade Producer is a class that can be created  the orderbook
// When an order is filled, a trade producer is sent
// A trade producer keeps track of remaining volume in the order
// All trades produced

class TradeProducer {

private:
    Order& order;
    std::vector<Trade> trades;

public:
    std::optional<Order> match_order(Order);

};

