
#pragma once

#define NUM_PRIORITIES 3
#include "buffer.h"

const static int NO_TASKS = -1;

class Scheduler {
    public:
        Scheduler();
        int get_next();
        void add_task(int priority, int task_id);
        
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
        int prepared_response;
        bool alive;
        bool initialized;
        void (*pc) (); // program counter, but typically only used as a reference value to see where the start of the program is
        char * sp; // stack pointer
        char* kernel_stack [4096]; // approximate 40 kbytes per stack

        void show_info(); // used for debug
        bool is_alive();
        bool kill();
};