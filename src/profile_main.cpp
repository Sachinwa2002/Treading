#include "exchange.h"
#include "order.h"
#include "logger.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <vector>

static int64_t percentile(std::vector<int64_t>& samples, int percentileValue) {
    const size_t index = (samples.size() - 1) * percentileValue / 100;
    return samples[index];
}

int main(){
    Logger::init();
    Exchange exchange;
    srand(42);

    // pre-fill with sell orders
    for(int i = 0; i < 50000; i++){
        Order sell;
        sell.orderId = i;
        sell.clientId = 100000 + i;
        sell.side = Side::SELL;
        sell.symbol = "TCS";
        sell.price = 200000 + rand() % 100000;
        sell.quantity = 10;
        sell.remainingQuantity = 10;
        sell.orderType = OrderType::LIMIT;
        sell.status = Status::NEW;
        sell.timeStamp = i;
        exchange.processOrder(sell);
    }

    // matching buy orders — the hot path we want to profile
    constexpr int orderCount = 500000;
    std::vector<int64_t> latencies;
    latencies.reserve(orderCount);

    for(int i = 0; i < orderCount; i++){
        Order buy;
        buy.orderId = 500000 + i;
        buy.clientId = 700000 + i;
        buy.side = Side::BUY;
        buy.symbol = "TCS";
        buy.price = 350000;
        buy.quantity = 10;
        buy.remainingQuantity = 10;
        buy.orderType = OrderType::LIMIT;
        buy.status = Status::NEW;
        buy.timeStamp = i;

        const auto start = std::chrono::steady_clock::now();
        exchange.processOrder(buy);
        const auto end = std::chrono::steady_clock::now();
        latencies.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    std::sort(latencies.begin(), latencies.end());
    const int64_t p50 = percentile(latencies, 50);
    const int64_t p90 = percentile(latencies, 90);
    const int64_t p99 = percentile(latencies, 99);
    std::cout << "Measured " << orderCount
              << " orders (construction and sell-order setup excluded; trade logging included)\n"
              << "p50: " << p50 << " ns\n"
              << "p90: " << p90 << " ns\n"
              << "p99: " << p99 << " ns\n";

    Logger::close();
    return 0;
}