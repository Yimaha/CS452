

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
void Yield();
// void Exit();


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
        uint16_t activate();

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


