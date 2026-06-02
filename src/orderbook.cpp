#include "orderbook.h"

void OrderBook::addOrder(Order order) {
    if (order.side == Side::BUY) {
        bids[order.price].push_back(order);
    } else {
        asks[order.price].push_back(order);
    }
    orderIndex[order.orderId] = {order.price, order.side};
}

void OrderBook::cancelOrder(uint64_t orderId) {
    cancelledOrders.insert(orderId);
    if (orderIndex.find(orderId) != orderIndex.end()) {
        int64_t price = orderIndex[orderId].price;
        if (orderIndex[orderId].side == Side::BUY) {
            for (auto it = bids[price].begin(); it != bids[price].end();) {
                if (it->orderId == orderId) {
                    it = bids[price].erase(it);
                    orderIndex.erase(orderId);
                    cancelledOrders.erase(orderId);
                    break;
                } else {
                    it++;
                }
            }
            if (bids[price].empty()) {
                bids.erase(price);
            }
        } else {
            for (auto it = asks[price].begin(); it != asks[price].end();) {
                if (it->orderId == orderId) {
                    it = asks[price].erase(it);
                    orderIndex.erase(orderId);
                    cancelledOrders.erase(orderId);
                    break;
                } else {
                    it++;
                }
            }
            if (asks[price].empty()) {
                asks.erase(price);
            }
        }
    }
}
Order OrderBook::emptyOrder = Order{};

Order& OrderBook::getBestBid() {
    if (bids.empty()) return emptyOrder;
    return bids.begin()->second.front();
}

Order& OrderBook::getBestAsk() {
    if (asks.empty()) return emptyOrder;
    return asks.begin()->second.front();
}

bool OrderBook::isCancelled(uint64_t orderId) {
    return cancelledOrders.count(orderId) > 0;
}

void OrderBook::removeTopBid() {
    if (bids.empty()) return;
    auto& dq = bids.begin()->second;
    dq.pop_front();
    if (dq.empty()) {
        bids.erase(bids.begin());
    }
}

void OrderBook::removeTopAsk() {
    if (asks.empty()) return;
    auto& dq = asks.begin()->second;
    dq.pop_front();
    if (dq.empty()) {
        asks.erase(asks.begin());
    }
}