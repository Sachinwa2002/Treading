#pragma once

#include "order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class OrderBook {

private:
    static Order emptyOrder;
    int64_t MIN_PRICE;
    int64_t MAX_PRICE;
    int64_t PRICE_LEVELS;
    std::vector<std::deque<Order>>bids;
    std::vector<std::deque<Order>>asks;
    std::unordered_map<uint64_t, OrderLocation> orderIndex;
    std::unordered_set<uint64_t> cancelledOrders;

    int64_t bestBidIndex;
    int64_t bestAskIndex;

public:
    OrderBook(int64_t minPrice,int64_t maxPrice):
    MIN_PRICE(minPrice),
    MAX_PRICE(maxPrice),
    PRICE_LEVELS(MAX_PRICE-MIN_PRICE+1),
    bids(PRICE_LEVELS),
    asks(PRICE_LEVELS),
    bestBidIndex(MIN_PRICE),
    bestAskIndex(MAX_PRICE){}

    void addOrder(Order &order);
    void cancelOrder(uint64_t orderId);
    Order& getBestBid();
    Order& getBestAsk();
    bool isCancelled(uint64_t orderId);
    void removeTopBid();
    void removeTopAsk();
};