
#pragma once


class Scheduler {
    public:
        Scheduler();
        int get_next();
        void add_task(int priority, int task_id);
        
    private:
        int ready_queue[1];
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