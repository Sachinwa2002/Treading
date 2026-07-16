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
#include "ringbuffer.h"

void consumerLoop(RingBuffer& rb, Exchange& ex) {
    Order order;
    while(true) {
        if(rb.pop(order)) {
            ex.processOrder(order);
        }
        // spin if empty
    }
}
int main(){
    Logger::init();

    RingBuffer ringBuffer;
    Exchange exchange;
    MarketDataFeed feed(ringBuffer,"BTCUSDT");
    feed.connectWebSocket();

    std::thread consumer(consumerLoop, std::ref(ringBuffer), std::ref(exchange));

    feed.runWebSocket();

    consumer.join();

    
    Logger::close();
    return 0;
}