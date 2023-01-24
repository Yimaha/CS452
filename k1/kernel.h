

#pragma once
#include <stdint.h>
#include <new>

#include "scheduler.h"
#include "context_switch.h"
#include "user_tasks.h"

#define KERNAL_STACK_FRAME 80000000


// trigger functions, written in assembly, should 
// int Create(int priority, void (*function)());
// int MyTid();
// int MyParentTid();
void yield();
// void Exit();

struct InterruptFrame {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t lr;
    uint64_t xzr;
}; 

class Kernel {
    public:
        enum HandlerCode {
            CREATE = 1,
            MY_TID = 2,
            MY_PARENT_ID = 3,
            YIELD = 4,
            EXIT = 5
        };
        Kernel();
        ~Kernel();
        void handle(uint16_t request);
        void schedule_next_task();
        InterruptFrame* activate();

    private:
        int p_id_counter = 0;  
        int active_task; 
        char* task_slab_address = (char*) 0x10000000;

        Scheduler scheduler;  // scheduler doesn't hold the actual task descrptor, simply an id and the priority
        TaskDescriptor* tasks[30]; // points to the starting location of taskDescriptors
        
    void allocate_new_task(int parent_id, int priority, void (*pc)());
        // int Create(int priority, void (*function)());
        // int MyTid();
        // int MyParentTid();
        // void Yield();
        // void Exit();
};


