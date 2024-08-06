#include "book_side.h"

#include <functional>

BookSide::BookSide(Side side, Price best_price, Price tick_size)
    : side_(side),
      best_price_(best_price),
      tick_size_(tick_size) {
    Price start_price_ = side_ == BUY ? best_price_ - (best_price_ / Price(20)) : best_price_ + (best_price_ / Price(20));
    tick_size_ = side_ == BUY ? tick_size_ : tick_size_ * Price(-1);

    for (int i = 0; i < allocate_size_; ++i) {
        levels_.emplace_back(start_price_);
        // levels_map_.emplace(start_price_, std::ref(levels_.back()));
        start_price_ += tick_size_;
    }
}

void BookSide::add_order(const Order &order) {

}

void BookSide::remove_order(OrderId id) {
}
