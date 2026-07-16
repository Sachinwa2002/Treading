#include<iostream>
#include "ringbuffer.h"
#include "logger.h"

bool RingBuffer::push(const Order &order){
    std::int64_t currTail=tail.load(std::memory_order_relaxed);
    std::int64_t currHead=head.load(std::memory_order_acquire);
    if(((currTail+1)&(SIZE-1))==currHead){
        return false;
    }
    else{
        buffer[currTail]=order;
        tail.store(((currTail+1) & (SIZE-1)),std::memory_order_release);
        return true;
    }
}

bool RingBuffer::pop(Order &order){
    std::int64_t currHead=head.load(std::memory_order_relaxed);
    std::int64_t currTail=tail.load(std::memory_order_acquire);
    if(currHead==currTail){
        return false;
    }
    else{
        order=buffer[currHead];
        head.store(((currHead+1) & (SIZE-1)),std::memory_order_release);
        return true;
    }
}
