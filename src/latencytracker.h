#pragma once
#include <iostream>
#include <chrono>
#include <vector>

class LatencyTracker{

    private:
    //we defined the power of 2 as the capacity of the latency tracker, which is 2^17=131072
    static const size_t CAPACITY=131072;
    std::vector<int64_t>latencies;
    std::vector<int64_t>copylatencies;
    size_t latencyCount=0;

    
    public:
    LatencyTracker(): latencies(CAPACITY,0){}

    void addLatency(int64_t latencyNs);
    void printStats();

};