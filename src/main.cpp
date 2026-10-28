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
            static int64_t counter=0;
            auto start=std::chrono::high_resolution_clock::now();
            ex.processOrder(order);
            auto end=std::chrono::high_resolution_clock::now();
           auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
           if(counter>=100){
              tracker.addLatency(ns);
           }
           counter++;
           if(counter%1000==0){
            tracker.printStats();
           }
           if(counter>=50000){
            tracker.printStats();
             std::exit(0);
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