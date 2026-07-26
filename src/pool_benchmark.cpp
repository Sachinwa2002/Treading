#include "memorypool.h"
#include "order.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
static void escape(void* p) {
    asm volatile("" : : "g"(p) : "memory");
}

int main(){
    const int N= 1000000;
    auto start1= std::chrono::high_resolution_clock::now();
    volatile void* sink=nullptr;

    for(int i=0;i<N; i++){
        void* p=malloc(sizeof(Order));
        escape(p);
        free(p);
    }
    auto end1= std::chrono::high_resolution_clock::now();
    auto mallocTime=std::chrono::duration_cast<std::chrono::nanoseconds>(end1-start1).count();
    
    std::cout<<"malloc/free: "<<mallocTime/N<<"ns per operation" <<std::endl;
    
    MemoryPool pool(sizeof(Order));


    auto start2=std::chrono::high_resolution_clock::now();

    for(int i=0;i<N;i++){
        void *p= pool.allocate();
        sink=p;
        pool.deallocate(p);
    }

    auto end2=std::chrono::high_resolution_clock::now();
    auto poolTime=std::chrono::duration_cast<std::chrono::nanoseconds>(end2-start2).count();
    std::cout<<"memory pool "<<poolTime/N <<"ns per operation "<<std::endl;


    std::cout << "malloc raw ns: " << mallocTime << std::endl;
    std::cout << "pool raw ns: " << poolTime << std::endl;
}