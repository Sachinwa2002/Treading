#include "logger.h"
#include<iostream>
#include <chrono>

std::ofstream Logger::tradeFile;
std::ofstream Logger::errorFile;

void Logger::init(){
    tradeFile.open("trades.log",std::ios::app);
    errorFile.open("errors.log",std::ios::app);
}

void Logger::logTrade(uint64_t buyOrderId, uint64_t sellOrderId, int64_t price,
                         uint64_t quantity, const std::string& symbol) {

     //write the trade logs here;
     auto now=std::chrono::system_clock::now();
     auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
     tradeFile<<"[" <<ms<< "][TRADE]"
              << "Symbol: " << symbol
              << " | BuyOrder: " << buyOrderId
              << " | SellOrder: " << sellOrderId
              << " | Price: " << price
              << " | Qty: " << quantity
              << "\n";
      
     tradeFile.flush();

}


void Logger::logError(const std::string& reason, uint64_t orderId, int64_t price,
                         const std::string& symbol) {
    //write the error logs here;
    //this time will be in MS and calculated from the 1 jan 1970 00:00:00 UTC computer scientist picked this time because it is easy to calculate and store in computer memory.
    auto now=std::chrono::system_clock::now();
    auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    errorFile <<"[" <<ms<< "][ERROR]"
              << "  Reason: "<<reason
              << " | OrderId: "<< orderId
              << " | Price: "<<price
              << " | symbol: "<<symbol
              << " \n ";
    errorFile.flush();
}

void Logger::close(){
    tradeFile.close();
    errorFile.close();
}