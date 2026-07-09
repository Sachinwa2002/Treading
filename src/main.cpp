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
#include "logger.h"

int main(){
    Logger::init();

    Exchange exchange;
    MarketDataFeed feed(exchange,"BTCUSDT");
    feed.connectWebSocket();
    feed.runWebSocket();
    
    Logger::close();
    return 0;
}