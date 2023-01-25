
#pragma once
#include "buffer.h"
#define NUM_PRIORITIES 3

extern const int NO_TASKS = -1;

class Scheduler {
    public:
        Scheduler();
        int get_next();
        void add_task(int priority, int task_id, int task_id_2);
        
    private:
        RingBuffer<int> ready_queue[NUM_PRIORITIES];
};

#include <cstdint>

class TaskDescriptor {
    public:
        TaskDescriptor();
        TaskDescriptor(int id, int parent_id, int priority, void (*pc)());
        int task_id;
        int parent_id; // id = -1 means no parent
        int priority;
        bool initialized;
        void (*pc) (); // program counter
        char * sp; // stack pointer
        char* kernel_stack [4096]; // approximate 40 kbytes per stack

        void show_info(); // used for debug
};