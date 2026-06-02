#pragma once

#include "orderbook.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <unordered_set>

class MatchingEngine{
    private:
    OrderBook& orderBook;

    public:
    MatchingEngine(OrderBook &book): orderBook(book){}

    void processOrder(Order& Order);
    void processOrderWithTiming(Order& order);

    private:
    void executeTrade(Order & buyOrder, Order &sellOrder, uint64_t quantity);

    };