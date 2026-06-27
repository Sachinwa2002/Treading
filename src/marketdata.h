#pragma once
#include <iostream>
#include "order.h"
#include <deque>
#include "exchange.h"

class MarketDataFeed{
    private:
    Exchange &exchange;
    uint64_t nextOrderId;
    std::string symbol;
    int processedCount = 0;

    public:
    MarketDataFeed(Exchange &ex, std::string sym):
    exchange(ex),nextOrderId(1),symbol(sym){}

    void fetchAndProcess();

    private:
    void processLevel(std::string price,std::string qty,Side side);

};