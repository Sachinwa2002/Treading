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
   
    public:
    void processOrder(Order &order);

};