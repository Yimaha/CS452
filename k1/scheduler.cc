#include "scheduler.h"
#include "rpi.h"


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
TaskDescriptor::TaskDescriptor(int id, int parent_id, int priority, void (*pc)()): task_id{id}, parent_id{parent_id}, initialized{false},
priority{priority}, pc{pc} {
    sp = (char*)&kernel_stack[4096]; // point to the end of the kernel_stack
}

void TaskDescriptor::show_info() {
    char m1[] = "printing status of TaskDescriptor \r\n";
    uart_puts(0, 0, m1, sizeof(m1) - 1); 
    char m2[] = "ID:";
    uart_puts(0, 0, m2, sizeof(m2) - 1);
    print_int(task_id);
    print("\r\n", 2);
    char m3[] = "Parent ID:";
    uart_puts(0, 0, m3, sizeof(m3) - 1);
    print_int(parent_id);
    print("\r\n", 2);
    char m4[] = "priority:";
    uart_puts(0, 0, m4, sizeof(m4) - 1);
    print_int(priority);
    print("\r\n", 2);
    char m5[] = "initialized:";
    uart_puts(0, 0, m5, sizeof(m5) - 1);
    print_int((uint64_t)initialized);
    print("\r\n", 2);
    char m6[] = "pc:";
    uart_puts(0, 0, m6, sizeof(m6) - 1);
    print_int((uint64_t)pc);
    print("\r\n", 2);
    char m7[] = "sp:";
    uart_puts(0, 0, m7, sizeof(m7) - 1);
    print_int((uint64_t)sp);
    print("\r\n", 2);
    char m8[] = "kernel_stack:";
    uart_puts(0, 0, m8, sizeof(m8) - 1);
    print_int((uint64_t)kernel_stack);
    print("\r\n", 2);
}