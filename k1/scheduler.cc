#include "scheduler.h"


Scheduler::Scheduler() {}

int Scheduler::get_next() {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!ready_queue[i].is_empty()) {
            return ready_queue[i].pop_front();
        }
    }

    return NO_TASKS; // no tasks to run
}

void Scheduler::add_task(int priority, int task_id) {
    ready_queue[priority].push_back(task_id);
}

TaskDescriptor::TaskDescriptor() {}
TaskDescriptor::TaskDescriptor(int id, int parent_id, int priority, void (*pc)()): task_id{id}, parent_id{parent_id}, 
priority{priority}, pc{pc} {
    sp = kernel_stack; // point to the start of the kernel_stack
}