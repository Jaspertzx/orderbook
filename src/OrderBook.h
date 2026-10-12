#pragma once
#include "Order.h"
#include <string>

class OrderBook {
public:
    // Returns the new order's id.
    uint64_t addOrder(Side side, double price, uint32_t quantity);
    // Returns true if the order was found and removed.
    bool cancelOrder(uint64_t id);
    // Human-readable snapshot of the book.
    std::string toString() const;

private:
    std::string name;
    uint64_t nextId_ = 1;
    // TODO: store bids/asks and match orders
};
