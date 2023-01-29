#pragma once
#include "rpi.h"
#include "utility.h"
#include <cstdint>

class TaskDescriptor
{
public:
    TaskDescriptor();
    TaskDescriptor(int id, int parent_id, int priority, void (*pc)());
    int task_id;
    int parent_id;            // id = -1 means no parent
    int priority;             // scheduler priority
    int prepared_response;    // used to store syscall return values
    bool alive;
    bool initialized;
    void (*pc)();             // program counter, but typically only used as a reference value to see where the start of the program is
    char *sp;                 // stack pointer
    char *kernel_stack[4096]; // approximately 4 kbytes per stack

    void show_info();         // used for debugging
    bool is_alive();
    bool kill();
};