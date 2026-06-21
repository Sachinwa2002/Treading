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
    // std::map<int64_t, std::deque<Order>, std::greater<int64_t>> bids;
    // std::map<int64_t, std::deque<Order>, std::less<int64_t>> asks;
    static const int64_t PRICE_LEVELS=MAX_PRICE-MIN_PRICE+1;
    std::vector<std::deque<Order>>bids;
    std::vector<std::deque<Order>>asks;
    std::unordered_map<uint64_t, OrderLocation> orderIndex;
    std::unordered_set<uint64_t> cancelledOrders;

    int64_t bestBidIndex;
    int64_t bestAskIndex;

public:
    OrderBook():
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