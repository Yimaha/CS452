
#pragma once
#include "buffer.h"
#define NUM_PRIORITIES 3

extern const int NO_TASKS = -1;

class Scheduler {
    public:
        Scheduler();
        int get_next();
        void add_task(int priority, int task_id);
        
    private:
        RingBuffer<int> ready_queue[NUM_PRIORITIES];
};


class TaskDescriptor {
    public:
        TaskDescriptor();
        TaskDescriptor(int id, int parent_id, int priority, void (*pc)());
        int task_id;
        int parent_id; // id = -1 means no parent
        int priority;
        void (*pc) (); // program counter
        char* sp; // stack pointer
        char kernel_stack [5000]; // approximate 50 kbytes per stack
};