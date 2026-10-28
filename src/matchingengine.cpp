#include<iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include "matchingengine.h"
#include "logger.h"


// Matching-engine level order processing.
// This function is responsible for matching a single incoming order against the current
// resting orders for that symbol. It checks cancellation, compares price against the best
// opposing order, executes trades when possible, and leaves unmatched LIMIT orders in the book.
// void MatchingEngine::processOrderWithTiming(Order &order){
//     auto start=std::chrono::high_resolution_clock::now();
//     processOrder(order);
//     auto end=std::chrono::high_resolution_clock::now();
//     auto ns= std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
//     std:: cout<<"Latency | orderId: "<<order.orderId<<" | " << ns <<"ns" <<std::endl;
// }
void MatchingEngine::processOrder(Order& order){
    // Check whether this order was cancelled before attempting matching.
    auto start=std::chrono::high_resolution_clock::now();

    if(orderBook.isCancelled(order.orderId)){
        auto end=std::chrono::high_resolution_clock::now();
        const auto ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        std::cout << "[LATENCY] orderId=" << order.orderId
                  << " matching=" << ns << " ns\n";
        return;
    }

    if(order.side==Side::BUY){
        while(order.remainingQuantity>0){
            Order& bestAsk=orderBook.getBestAsk();
            //1st check point is the ask exist or not
            if(bestAsk.orderId==0){
                //if best ask is empty then we will add the order into orderbook according to its type
                if(order.orderType== OrderType:: LIMIT){
                     orderBook.addOrder(order);
                     Logger::logOrderEvent("RESTING", order);
                     //std::cout<<"Order WAITING in orderBook | orderId: " << order.orderId << " | qty : " << order.remainingQuantity << " | price : " << order.price << std::endl;
                }
                break;
            }

            //2nd check point check resting order.
            if(orderBook.isCancelled(bestAsk.orderId)){
                orderBook.removeTopAsk();
                continue;
            }

            //3rd check point for the price and market type
             if(order.orderType == OrderType::LIMIT && bestAsk.price>order.price){
                 orderBook.addOrder(order);
                 Logger::logOrderEvent("RESTING", order);
                 //std::cout<<"Order WAITING in orderBook  | orderId: " << order.orderId << " | qty : " << order.remainingQuantity << " | price : " << order.price << std::endl;
                 break;
            }
            else{
                  uint64_t tradeQuantity=std::min(order.remainingQuantity,bestAsk.remainingQuantity);
                  executeTrade(order,bestAsk,tradeQuantity);
            }
            if(bestAsk.remainingQuantity==0){
                orderBook.removeTopAsk();
            }
        }
        if(order.remainingQuantity>0 && order.orderType==OrderType::MARKET){
            order.status=Status::CANCELLED;
            Logger::logOrderEvent("MARKET_REMAINDER_CANCELLED", order);
            //std::cout<<"MARKET Order Cancelled | orderId: " << order.orderId << " | qty : " << order.remainingQuantity << " | price : " << order.price << std::endl;
        }
      
    }
    else{
        while(order.remainingQuantity>0){
            Order& bestBid=orderBook.getBestBid();

            //1st check if the bid exist or not
            if(bestBid.orderId==0){
                //if best bid is empty then we will add the order into orderbook according to its type
                if(order.orderType== OrderType:: LIMIT){
                     orderBook.addOrder(order);
                     Logger::logOrderEvent("RESTING", order);
                     //std::cout<<"Order Waiting in the orderBook | orderId: " <<order.orderId << " | qty : "<< order.remainingQuantity << " | price : "<<order.price<<std::endl;
                }
                break ;
            }   

            //2nd check point check resting order.
            if(orderBook.isCancelled(bestBid.orderId)){
                orderBook.removeTopBid();
                continue;
            }

            //3rd check for the market type and price.
            if(order.orderType==OrderType::LIMIT && bestBid.price<order.price){
                //if the best bid price is less then for LIMIT order we push them into the cart
                orderBook.addOrder(order);
                Logger::logOrderEvent("RESTING", order);
                //  std::cout<<"Order Waiting in the orderBook | orderId: " <<order.orderId << " | qty : "<< order.remainingQuantity << " | price : "<<order.price<<std::endl;
                    break;
            }
            else{
                  uint64_t tradeQuantity=std::min(order.remainingQuantity,bestBid.remainingQuantity);
                  executeTrade(bestBid,order,tradeQuantity);
            }
            if(bestBid.remainingQuantity==0){
                orderBook.removeTopBid();
            }
        }
        if(order.remainingQuantity>0 && order.orderType==OrderType::MARKET){
            order.status=Status::CANCELLED;
            Logger::logOrderEvent("MARKET_REMAINDER_CANCELLED", order);
            //std::cout<<"MARKET Order Cancelled | orderId: " << order.orderId << " | qty : " << order.remainingQuantity << " | price : " << order.price << std::endl;
        }
    }
    auto end=std::chrono::high_resolution_clock::now();
    const auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    std::cout << "[LATENCY] orderId=" << order.orderId
              << " matching=" << ns << " ns\n";
}

void MatchingEngine::executeTrade(Order & buyOrder, Order &sellOrder, uint64_t quantity){
    buyOrder.remainingQuantity=buyOrder.remainingQuantity-quantity;
    sellOrder.remainingQuantity=sellOrder.remainingQuantity-quantity;

    if(buyOrder.remainingQuantity==0){
        buyOrder.status=Status::FILLED;
    }
    else{
        buyOrder.status= Status::PARTIAL_FILL;
        //std::cout << "BUY ORDER PARTIAL FILL | orderId: " << buyOrder.orderId << " | remaining: " << buyOrder.remainingQuantity << std::endl;
    }

    if(sellOrder.remainingQuantity==0){
        sellOrder.status=Status::FILLED;
    }
    else{
        sellOrder.status=Status::PARTIAL_FILL;
        //std::cout << "SELL ORDER PARTIAL FILL | orderId: " << sellOrder.orderId << " | remaining: " << sellOrder.remainingQuantity << std::endl;
    }
    Logger::logTrade(buyOrder.orderId, sellOrder.orderId,
                 sellOrder.price, quantity, sellOrder.symbol,
                 buyOrder.remainingQuantity, sellOrder.remainingQuantity);
           
}
