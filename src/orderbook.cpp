#include "orderbook.h"

void OrderBook::addOrder(Order &order) {
    if (order.side == Side::BUY) {
        if(order.price>bestBidIndex){
            bestBidIndex=order.price;
        }
        bids[order.price-MIN_PRICE].push_back(order);
    } else {
        if(order.price<bestAskIndex){
            bestAskIndex=order.price;
        }
        asks[order.price-MIN_PRICE].push_back(order);
    }
    orderIndex[order.orderId] = {order.price, order.side};
}

void OrderBook::cancelOrder(uint64_t orderId) {
    cancelledOrders.insert(orderId);
    if (orderIndex.find(orderId) != orderIndex.end()) {
        int64_t price = orderIndex[orderId].price;
        Side cancellSide=orderIndex[orderId].side;
        if (cancellSide == Side::BUY) {
            for (auto it = bids[price-MIN_PRICE].begin(); it != bids[price-MIN_PRICE].end();) {
                if (it->orderId == orderId) {
                    it = bids[price-MIN_PRICE].erase(it);
                    orderIndex.erase(orderId);
                    cancelledOrders.erase(orderId);
                    break;
                } else {
                    it++;
                }
            }
        } else {
            for (auto it = asks[price-MIN_PRICE].begin(); it != asks[price-MIN_PRICE].end();) {
                if (it->orderId == orderId) {
                    it = asks[price-MIN_PRICE].erase(it);
                    orderIndex.erase(orderId);
                    cancelledOrders.erase(orderId);
                    break;
                } else {
                    it++;
                }
            }
        }
    }
}
Order OrderBook::emptyOrder = Order{};

Order& OrderBook::getBestBid() {
    if (bids[bestBidIndex-MIN_PRICE].empty()) return emptyOrder;
    return bids[bestBidIndex-MIN_PRICE].front();
}

Order& OrderBook::getBestAsk() {
    if (asks[bestAskIndex-MIN_PRICE].empty()) return emptyOrder;
    return asks[bestAskIndex-MIN_PRICE].front();
}

bool OrderBook::isCancelled(uint64_t orderId) {
    return cancelledOrders.count(orderId) > 0;
}

void OrderBook::removeTopBid() {
    if (bids[bestBidIndex-MIN_PRICE].empty()) return;
    auto& dq = bids[bestBidIndex-MIN_PRICE];
    dq.pop_front();
    while(bestBidIndex>MIN_PRICE && bids[bestBidIndex-MIN_PRICE].empty()){
        bestBidIndex--;
    }
    
}

void OrderBook::removeTopAsk() {
    if (asks[bestAskIndex-MIN_PRICE].empty()) return;
    auto& dq = asks[bestAskIndex-MIN_PRICE];
    dq.pop_front();
    while(bestAskIndex<MAX_PRICE && asks[bestAskIndex-MIN_PRICE].empty()){
        bestAskIndex++;
    }
}