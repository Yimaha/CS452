#include "scheduler.h"


Scheduler::Scheduler() {}

int Scheduler::get_next() {
    return ready_queue[0];
}

void Scheduler::add_task(int priority, int task_id) {
    ready_queue[0] = task_id;
}

TaskDescriptor::TaskDescriptor() {}
TaskDescriptor::TaskDescriptor(int id, int parent_id, int priority, void (*pc)()): task_id{id}, parent_id{parent_id}, initialized{false},
priority{priority}, pc{pc} {
    sp = kernel_stack; // point to the start of the kernel_stack
}