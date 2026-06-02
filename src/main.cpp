// 1. Create OrderBook
// 2. Create MatchingEngine
// 3. Create a SELL order and process it
// 4. Create a BUY order and process it
// 5. Print whether they matched

#include<iostream>
#include "order.h"
#include "matchingengine.h"

int main(){
    OrderBook book;
    MatchingEngine engine(book);
    Order sellOrder;
    sellOrder.orderId = 1;
    sellOrder.clientId = 101;
    sellOrder.side = Side::SELL;
    sellOrder.symbol = "TCS";
    sellOrder.price = 350000;    // ₹3500 in paise
    sellOrder.quantity = 100;
    sellOrder.remainingQuantity = 100;
    sellOrder.orderType = OrderType::LIMIT;
    sellOrder.status = Status::NEW;
    sellOrder.timeStamp = 1000;

    engine.processOrderWithTiming(sellOrder);

    Order buyOrder;
    buyOrder.orderId = 3;
    buyOrder.clientId = 102;
    buyOrder.side = Side::BUY;
    buyOrder.symbol = "TCS";
    buyOrder.price = 350000;
    buyOrder.quantity = 100;
    buyOrder.remainingQuantity = 100;
    buyOrder.orderType = OrderType::LIMIT;
    buyOrder.status = Status::NEW;
    buyOrder.timeStamp = 2000;

    engine.processOrderWithTiming(buyOrder);


    return 0;
}