#pragma once
#include "iostream"
#include "order.h"
#include<vector>
#include<atomic>

class RingBuffer{
    private:
    static const size_t SIZE=1024;
    Order buffer[SIZE];
    std::atomic<std::int64_t>head;
    std::atomic<std::int64_t>tail;

    public:
    RingBuffer(): head(0),tail(0){}

    bool push(const Order &order);
    bool pop(Order &order);

 
    
};