#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>


class Logger{
    private:
    static std::ofstream tradeFile;
    static std::ofstream errorFile;

    public:
    static void init();

    static void logTrade(uint64_t buyOrderId,uint64_t sellOrderId,int64_t price,
                         uint64_t quantity, const std::string& symbol);

    
    static void logError(const std::string& reason, uint64_t orderId,int64_t price,
                         const std::string& symbol);

    static void close();

};
