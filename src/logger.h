#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>
#include "order.h"


class Logger{
    private:
    static std::ofstream tradeFile;
    static std::ofstream errorFile;
    static std::ofstream historyFile;

    public:
    static void init();

    static void logTrade(uint64_t buyOrderId,uint64_t sellOrderId,int64_t price,
                         uint64_t quantity, const std::string& symbol,
                         uint64_t buyRemaining, uint64_t sellRemaining);

    static void logOrderEvent(const std::string& event, const Order& order,
                              const std::string& details = "");

    
    static void logError(const std::string& reason, uint64_t orderId,int64_t price,
                         const std::string& symbol);

    static void close();

};
