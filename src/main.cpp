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
#include "latencytracker.h"

void consumerLoop(RingBuffer& rb, Exchange& ex,LatencyTracker &tracker){
    Order order;
    while(true) {
        if(rb.pop(order)) {
            auto start=std::chrono::high_resolution_clock::now();
            ex.processOrder(order);
            auto end=std::chrono::high_resolution_clock::now();
           auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
           tracker.addLatency(ns);

           static int64_t counter=0;
           counter++;
           if(counter%20==0){
            tracker.printStats();
           }
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
    LatencyTracker tracker;

    std::thread consumer(consumerLoop, std::ref(ringBuffer), std::ref(exchange),std::ref(tracker));

    feed.runWebSocket();

    consumer.join();

    
    Logger::close();
    return 0;
}