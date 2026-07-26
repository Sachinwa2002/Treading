#include "memorypool.h"

MemoryPool::MemoryPool(size_t objectSize) {
    slotSize = objectSize;
    pool = new char[POOL_SIZE * slotSize];
    
    // TODO: loop through all slots, link each to the next
    // last slot should point to nullptr

    for(size_t i=0;i<POOL_SIZE-1;i++){
        char* slotPtr=pool+(i*slotSize);
        char* nextSlotPtr=pool+(i+1)*slotSize;

        *reinterpret_cast<void**>(slotPtr) = nextSlotPtr;
    }
    //last slot pointer should be nullPtr
    char* slotPtr=pool+(POOL_SIZE-1)*slotSize;
    *reinterpret_cast<void**>(slotPtr) = nullptr;
    
    freeListHead = pool;  // first free slot is slot 0
}

MemoryPool::~MemoryPool(){
    delete [] pool;
}

void* MemoryPool::allocate(){
    if(freeListHead==nullptr){
        return nullptr; // no free slots
    }
    else{
        void* slot=freeListHead;
        void* next=*reinterpret_cast<void**>(freeListHead);
        freeListHead=next;
        return slot;
    }
}

void MemoryPool::deallocate(void *ptr){
    void* prev= ptr;
    *reinterpret_cast<void**>(prev) = freeListHead;
    freeListHead = ptr;
}