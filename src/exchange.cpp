#include "exchange.h"
#include  <iostream>
#include "orderbook.h"
#include "logger.h"

void Exchange::processOrder(Order &order){
if(symbolConfig.find(order.symbol)==symbolConfig.end()){
    Logger::logError("Invalid order symbol", order.orderId, order.price, order.symbol);
    return ;
}
    int64_t symMax=symbolConfig[order.symbol].second;
    int64_t symMin=symbolConfig[order.symbol].first;

if(order.price<0 || order.price>symMax || order.price<symMin){
    Logger::logError("Invalid order price", order.orderId, order.price, order.symbol);
    return ;
}
if(order.quantity<=0){
    Logger::logError("Invalid order quantity", order.orderId, order.price, order.symbol);
    return ;
}
if(order.symbol.empty()){
    Logger::logError("Invalid order symbol", order.orderId, order.price, order.symbol);
    return ;
}
if(order.remainingQuantity!=order.quantity){
    Logger::logError("Order remaining quantity and order quantity mismatch", order.orderId, order.price, order.symbol);
    return ;
}

    //check if the orderBooks has the OrderBook for this specific symbol or not
    if(orderBooks.find(order.symbol)==orderBooks.end()){
        int64_t currminprice=symbolConfig[order.symbol].first;
        int64_t currmaxprice=symbolConfig[order.symbol].second;
        orderBooks[order.symbol]=std::make_unique<OrderBook>(currminprice,currmaxprice);
        engines[order.symbol]=std::make_unique<MatchingEngine>(*orderBooks[order.symbol]);
    }
   //get enginee for this order
   MatchingEngine *enginee=engines[order.symbol].get();

   //process the order to the matching enginee
   enginee->processOrder(order);
}