#include "OrderBook.h"

uint64_t OrderBook::addOrder(Side side, double price, uint32_t quantity) {
    Order order{nextId_++, side, price, quantity};
    // TODO: match against the opposite side, rest the remainder in the book
    (void)order;
    return order.id;
}

bool OrderBook::cancelOrder(uint64_t id) {
    // TODO: find and remove the order
    (void)id;
    return false;
}

std::string OrderBook::toString() const {
    // TODO: print bids and asks
    return "(empty book)\n";
}
