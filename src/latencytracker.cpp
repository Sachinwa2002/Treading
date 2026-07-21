#include <iostream>
#include "latencytracker.h"
#include <algorithm>

void LatencyTracker::addLatency(int64_t latencyNs){
    latencies[(latencyCount) & (CAPACITY-1)]=latencyNs;
    latencyCount++;
}

void LatencyTracker::printStats(){
    if(latencyCount==0){
      std::cout<<"No latency data available."<<std::endl;
    }
    else{
           copylatencies=latencies;
           std::sort(copylatencies.begin(), copylatencies.begin()+std::min(latencyCount,CAPACITY));
           int64_t p99Index= std::min(latencyCount,CAPACITY)*0.99;
           int64_t p50Index= std::min(latencyCount,CAPACITY)*0.50;
           int64_t p90Index= std::min(latencyCount,CAPACITY)*0.90;
           std::cout<<"the p50 Latency is: "<<copylatencies[p50Index]<<std::endl;
           std::cout<<"the p90 Latency is: "<<copylatencies[p90Index]<<std::endl;
           std::cout<<"the p99 Latency is: "<<copylatencies[p99Index]<<std::endl;
    }
}