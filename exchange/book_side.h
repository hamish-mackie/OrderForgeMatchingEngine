#pragma once

#include <vector>
#include <deque>

#include "book_level.h"

class BookSide {
public:
    using Levels = std::deque<BookLevel>;
    using LevelsMap = std::unordered_map<Price, BookLevel&>;
    using AllocateSize = uint64_t;

    AllocateSize allocate_size_{10000};

    BookSide(Side side, Price best_price, Price tick_size);

    void add_order(const Order& order);
    void remove_order(OrderId id);
    void match_order(Order& order);

    Price start_price() { return levels_.begin()->price(); }
    Price end_price() { return (levels_.end()-1)->price(); }

private:
    Side side_;
    Price best_price_;
    Price tick_size_;
    Levels levels_;
    // LevelsMap levels_map_;
};

