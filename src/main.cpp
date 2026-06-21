// 1. Create OrderBook
// 2. Create MatchingEngine
// 3. Create a SELL order and process it
// 4. Create a BUY order and process it
// 5. Print whether they matched

#include<iostream>
#include "order.h"
#include "matchingengine.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "exchange.h"

int main(){
    Exchange exchange;
    
    srand(time(0));  // ✅ once only!
    
    // TCS SELL orders
    for(int i = 0; i < 100000; i++){
        Order sell;
        sell.orderId          = i;  // 0-99999
        sell.clientId         = 100000 + i;
        sell.side             = Side::SELL;
        sell.symbol           = "TCS";
        sell.price            = 300000 + rand()%60001;
        sell.quantity         = 10;
        sell.remainingQuantity = 10;
        sell.orderType        = OrderType::LIMIT;
        sell.status           = Status::NEW;
        sell.timeStamp        = i;
        exchange.processOrder(sell);
    }

    // TCS BUY orders + latency
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100000; i++){
        Order buy;
        buy.orderId           = 200000 + i;  // 200000-299999
        buy.clientId          = 300000 + i;
        buy.side              = Side::BUY;
        buy.symbol            = "TCS";
        buy.price             = 380000 + rand()%60001;
        buy.quantity          = 10;
        buy.remainingQuantity = 10;
        buy.orderType         = OrderType::LIMIT;
        buy.status            = Status::NEW;
        buy.timeStamp         = i;
        exchange.processOrder(buy);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns  = std::chrono::duration_cast
               <std::chrono::nanoseconds>(end-start).count();
    std::cout << "TCS average latency: " 
              << ns/100000 << "ns" << std::endl;

    // INFY SELL orders
    for(int i = 0; i < 100000; i++){
        Order sell;
        sell.orderId          = 400000 + i;  // ✅ different range!
        sell.clientId         = 500000 + i;
        sell.side             = Side::SELL;
        sell.symbol           = "INFY";
        sell.price            = 600000 + rand()%60001;
        sell.quantity         = 100;
        sell.remainingQuantity = 100;
        sell.orderType        = OrderType::LIMIT;
        sell.status           = Status::NEW;
        sell.timeStamp        = i;
        exchange.processOrder(sell);
    }

    // INFY BUY orders + latency
    auto start2 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100000; i++){
        Order buy;
        buy.orderId           = 600000 + i;  // ✅ different range!
        buy.clientId          = 700000 + i;
        buy.side              = Side::BUY;
        buy.symbol            = "INFY";
        buy.price             = 680000 + rand()%60001;
        buy.quantity          = 100;
        buy.remainingQuantity = 100;
        buy.orderType         = OrderType::LIMIT;
        buy.status            = Status::NEW;
        buy.timeStamp         = i;
        exchange.processOrder(buy);
    }
    auto end2 = std::chrono::high_resolution_clock::now();
    auto ns2  = std::chrono::duration_cast
                <std::chrono::nanoseconds>(end2-start2).count();
    std::cout << "INFY average latency: " 
              << ns2/100000 << "ns" << std::endl;


    //HCL sell orders
    for(int i = 0; i < 100000; i++){
        Order sell;
        sell.orderId          = 800000 + i;  // ✅ different range!
        sell.clientId         = 900000 + i;
        sell.side             = Side::SELL;
        sell.symbol           = "HCL";
        sell.price            = 400000 + rand()%60001;
        sell.quantity         = 100;
        sell.remainingQuantity = 100;
        sell.orderType        = OrderType::LIMIT;
        sell.status           = Status::NEW;
        sell.timeStamp        = i;
        exchange.processOrder(sell);
    }

    //HCL buy orders + latency
    auto start3= std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100000; i++){
        Order buy;
        buy.orderId           = 1000000 + i;  // ✅ different range!
        buy.clientId          = 1100000 + i;
        buy.side              = Side::BUY;
        buy.symbol            = "HCL";
        buy.price             = 480000 + rand()%60001;
        buy.quantity          = 100;
        buy.remainingQuantity = 100;
        buy.orderType         = OrderType::LIMIT;
        buy.status            = Status::NEW;
        buy.timeStamp         = i;
        exchange.processOrder(buy);
    }
    auto end3=std::chrono::high_resolution_clock::now();
    auto ns3=std::chrono::duration_cast<std::chrono::nanoseconds>(end3-start3).count();
    std::cout<<"HCL average latency: "<<ns/100000<<"ns"<<std::endl;




    return 0;
}