#pragma once
#include <iostream>
#include <cstddef>



// void* is a "generic pointer" — points to raw memory,
// no type attached.

// A memory pool doesn't care WHAT you store in the slot.
// It just manages raw memory blocks of a fixed size.

class MemoryPool{
    private:
    static const size_t POOL_SIZE=1000000;
    char* pool;
    void* freeListHead;
    size_t slotSize;


    public:
    MemoryPool(size_t objectSize);   // sets up the pool
    ~MemoryPool();                   // frees the pool



    void* allocate();
    void deallocate(void* ptr);
};