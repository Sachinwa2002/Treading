//creade exchange object 

#include<iostream>
#include "order.h"
#include "matchingengine.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "exchange.h"
#include "marketdata.h"
#include <thread>

int main(){

    Exchange exchange;
    MarketDataFeed feed(exchange,"BTCUSDT");
    for(int i=0;i<10;i++){
        feed.fetchAndProcess();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
    std::cout << "Market data processed" << std::endl;
    return 0;
}