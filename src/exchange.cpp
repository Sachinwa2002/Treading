#include "exchange.h"
#include "orderbook.h"
#include "logger.h"
#include <chrono>
#include <cstdint>
#include <iostream>

// Exchange-level order entry point.
// Validates the incoming order, creates the symbol's OrderBook/MatchingEngine if needed,
// and then hands the order to the matching engine for execution against existing bids/asks.
void Exchange::processOrder(Order &order){    
    Logger::logOrderEvent("ARRIVED", order);
    auto start=std::chrono::high_resolution_clock::now();
    const auto reportLatency = [&]() {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto exchangeNs =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        std::cout << "[LATENCY] orderId=" << order.orderId
                  << " exchange=" << exchangeNs << " ns\n";
    };

    auto cfgit=symbolConfig.find(order.symbol);
if(cfgit==symbolConfig.end()){
    Logger::logError("Invalid order symbol", order.orderId, order.price, order.symbol);
    Logger::logOrderEvent("REJECTED", order, "Reason=invalid symbol");
    reportLatency();
    return ;
}
    int64_t symMax=cfgit->second.second;
    int64_t symMin=cfgit->second.first;

if(order.price<0 || order.price>symMax || order.price<symMin){
    Logger::logError("Invalid order price", order.orderId, order.price, order.symbol);
    Logger::logOrderEvent("REJECTED", order, "Reason=invalid price");
    reportLatency();
    return ;
}
if(order.quantity<=0){
    Logger::logError("Invalid order quantity", order.orderId, order.price, order.symbol);
    Logger::logOrderEvent("REJECTED", order, "Reason=invalid quantity");
    reportLatency();
    return ;
}
if(order.symbol.empty()){
    Logger::logError("Invalid order symbol", order.orderId, order.price, order.symbol);
    Logger::logOrderEvent("REJECTED", order, "Reason=empty symbol");
    reportLatency();
    return ;
}
if(order.remainingQuantity!=order.quantity){
    Logger::logError("Order remaining quantity and order quantity mismatch", order.orderId, order.price, order.symbol);
    Logger::logOrderEvent("REJECTED", order, "Reason=remaining quantity mismatch");
    reportLatency();
    return ;
}

     auto engit=engines.find(order.symbol);
     if(engit==engines.end()){
        int64_t currminprice=cfgit->second.first;
        int64_t currmaxprice=cfgit->second.second;
        auto ob=std::make_unique<OrderBook>(currminprice,currmaxprice);
        auto me=std::make_unique<MatchingEngine>(*ob);

        orderBooks[order.symbol]=std::move(ob);
        engines[order.symbol]=std::move(me);
        engit=engines.find(order.symbol);
     }
     //process the order to the matching enginee
     engit->second->processOrder(order);
     reportLatency();
}