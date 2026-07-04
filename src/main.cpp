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
    feed.connectWebSocket();
    feed.runWebSocket();
    
    std::cout << "Market data processed" << std::endl;
    return 0;
}