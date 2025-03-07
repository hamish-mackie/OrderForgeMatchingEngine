#pragma once

#include <map>

#include "price_level.h"

namespace of {

class OrderBookDataInterface {
public:
    using PriceLvlPtr = PriceLevel*;

    OrderBookDataInterface(Side side, TickSize tick_size) : side_(side), tick_size_(tick_size) {}

    virtual ~OrderBookDataInterface() = 0;

    virtual Price best_price() = 0;
    virtual PriceLvlPtr get_book_level(const Price& price) = 0;
    virtual PriceLvlPtr add_book_level(const Price& price) = 0;
    virtual void remove_book_level(const Price &price) = 0;

protected:
    const Side side_;

private:
    virtual void generate_data_structure() = 0;
    const TickSize tick_size_;
};

// define pure virtual destructor
inline OrderBookDataInterface::~OrderBookDataInterface() {}

template<typename CompFunc>
class OrderBookDataMap : public OrderBookDataInterface {
public:
    using OrderBookContainer = std::map<Price, PriceLvlPtr, CompFunc>;

    OrderBookDataMap(Symbol symbol, Side side, const TickSize &tick_size) :
        OrderBookDataInterface(side, tick_size), symbol_(symbol), book_cont_() {}

    Price best_price() override { return book_cont_.begin()->first; };

    PriceLvlPtr get_book_level(const Price& price) override {
        auto it = book_cont_.find(price);
        if (it != book_cont_.end()) {
            return it->second;
        } else {
            return add_book_level(price);
        }
    };

    PriceLvlPtr add_book_level(const Price& price) override {
        auto res = book_cont_.emplace(price, new PriceLevel(symbol_, price, side_));
        return res.first->second;
    };

    void remove_book_level(const Price &price) override { auto res = book_cont_.erase(price); }

    ~OrderBookDataMap() override {
        for (auto r: book_cont_) {
            delete r.second;
        }
    };

    typename OrderBookContainer::iterator begin() { return book_cont_.begin(); }
    typename OrderBookContainer::iterator end() { return book_cont_.end(); }
    bool empty() { return book_cont_.empty(); }

private:
    Symbol symbol_;
    void generate_data_structure() override{};
    OrderBookContainer book_cont_;
};
} // namespace of
