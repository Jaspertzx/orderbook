#include "OrderBook.h"
#include <gtest/gtest.h>

TEST(OrderBook, AddOrderReturnsIncreasingIds) {
    OrderBook book;
    uint64_t a = book.addOrder(Side::Buy, 100.0, 10);
    uint64_t b = book.addOrder(Side::Sell, 101.0, 5);
    EXPECT_LT(a, b);
}

TEST(OrderBook, CancelUnknownOrderFails) {
    OrderBook book;
    EXPECT_FALSE(book.cancelOrder(12345));
}

// TODO: once the book stores orders, enable this
TEST(OrderBook, DISABLED_CancelExistingOrderSucceeds) {
    OrderBook book;
    uint64_t id = book.addOrder(Side::Buy, 100.0, 10);
    EXPECT_TRUE(book.cancelOrder(id));
    EXPECT_FALSE(book.cancelOrder(id));
}
