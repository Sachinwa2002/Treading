#include "exchange.h"
#include  <iostream>
#include "orderbook.h"
void Exchange::processOrder(Order &order){
if(symbolConfig.find(order.symbol)==symbolConfig.end()){
    std::cout<<"Invalid order symbol. order rejected | orderId:"<<order.orderId<< " |symbol :"<<order.symbol<<std::endl;
    return ;
}
    int64_t symMax=symbolConfig[order.symbol].second;
    int64_t symMin=symbolConfig[order.symbol].first;

if(order.price<0 || order.price>symMax || order.price<symMin){
    std::cout<<"Invalid order price. order rejected | orderId:"<<order.orderId<< " |price :"<<order.price<<std::endl;
    return ;
}
if(order.quantity<=0){
    std::cout<<"Invalid order quantity. order rejected | orderId:"<<order.orderId<< " |quantity :"<<order.quantity<<std::endl;
    return ;
}
if(order.symbol.empty()){
    std::cout<<"Invalid order symbol. order rejected | orderId:"<<order.orderId<< " |symbol :"<<order.symbol<<std::endl;
    return ;
}
if(order.remainingQuantity!=order.quantity){
    std::cout<<" order remaining quantity and order quantity mismatch. order rejected | orderId:"<<order.orderId<< " |remainingQuantity :"<<order.remainingQuantity<<std::endl;
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