#pragma once

#include "order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <unordered_set>

class OrderBook {

private:
    static Order emptyOrder;
    std::map<int64_t, std::deque<Order>, std::greater<int64_t>> bids;
    std::map<int64_t, std::deque<Order>, std::less<int64_t>> asks;
    std::unordered_map<uint64_t, OrderLocation> orderIndex;
    std::unordered_set<uint64_t> cancelledOrders;

public:
    void addOrder(Order order);
    void cancelOrder(uint64_t orderId);
    Order& getBestBid();
    Order& getBestAsk();
    bool isCancelled(uint64_t orderId);
    void removeTopBid();
    void removeTopAsk();
};