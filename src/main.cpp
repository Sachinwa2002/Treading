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

int main(){
    OrderBook book;
    MatchingEngine engine(book);
    
    srand(time(0));
    for(int i=0;i<100000;i++){
        Order sell;
        sell.orderId = i;
        sell.clientId= 100000+i;
        sell.side=Side::SELL;
        sell.symbol="TCS";
        sell.price= 300000 + rand()%60001 ;
        sell.quantity=10;
        sell.remainingQuantity=10;
        sell.orderType=OrderType::LIMIT;
        sell.status=Status::NEW;
        sell.timeStamp=i;
        engine.processOrder(sell);
    }


    auto start = std::chrono::high_resolution_clock::now();

    for(int i=0;i<100000;i++){
       Order buy;
       buy.orderId=200000+i;
       buy.clientId=300000+i;
       buy.side=Side::BUY;
       buy.symbol="TCS";
       buy.price=380000+rand()%60001;
       buy.quantity=10;
       buy.remainingQuantity=10;
       buy.orderType=OrderType::LIMIT;
       buy.status=Status::NEW;
       buy.timeStamp=i;
       engine.processOrder(buy);
    }
    auto end= std::chrono::high_resolution_clock::now();
    auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();\
    std::cout<<"average latency is : " << ns/100000<<"ns"<<std::endl;

    return 0;
}