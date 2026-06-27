#pragma once
#include "orderbook.h"
#include <unordered_map>
#include <memory>
#include <string>
#include "matchingengine.h"
#include "order.h"

class Exchange{
    private:
    std::unordered_map<std::string,std::unique_ptr<OrderBook>>orderBooks;
    std::unordered_map<std::string,std::unique_ptr<MatchingEngine>>engines;
    std::unordered_map<std::string, std::pair<int64_t, int64_t>> symbolConfig = {
    {"BTCUSDT", {6000000, 6100000}},
    {"TCS",     {200000,  800000}},
    {"INFY",    {100000,  400000}}
};
   
    public:
    void processOrder(Order &order);

};