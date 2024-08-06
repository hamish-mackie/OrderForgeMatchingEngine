#pragma once

#include <string>
#include <unordered_map>

#include "book_level.h"

class BookSide {
public:
    using OrderIdentifier = std::string;
    using OrdersMap = std::unordered_map<OrderIdentifier, Order*>;
private:
    void add_order(Order& order);
    void match_order(Order& order);
};

