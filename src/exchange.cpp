#include "exchange.h"
#include  <iostream>
#include "orderbook.h"
void Exchange::processOrder(Order &order){

//     struct Order {
//     Side side;
//     std::string symbol;
//     int64_t price;
//     uint64_t quantity;
//     OrderType orderType;
//     uint64_t orderId;
//     uint64_t clientId;
//     uint64_t timeStamp;
//     uint64_t remainingQuantity;
//     Status status;
// };
  
if(order.price<0 || order.price>MAX_PRICE){
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
        orderBooks[order.symbol]=std::make_unique<OrderBook>();
        engines[order.symbol]=std::make_unique<MatchingEngine>(*orderBooks[order.symbol]);
    }
   //get enginee for this order
   MatchingEngine *enginee=engines[order.symbol].get();

   //process the order to the matching enginee
   enginee->processOrder(order);


}