#include "kernel.h"
#include "rpi.h"


extern "C" void handle_syscall() {

}

void yield() {
    
};



Kernel::Kernel() {
    scheduler.add_task(0, p_id_counter);
    allocate_new_task(-1, 0, &Task_0);
    active_task = 0;
}

void Kernel::schedule_next_task() {
//    active_task = &tasks[scheduler.get_next()];
}

uint16_t Kernel::activate() {
    return first_el0_entry(tasks[active_task]->sp, tasks[active_task]->pc); // startup task, has no parameter or handling
}


Kernel::~Kernel() {}


void Kernel::handle(uint16_t request) {
    // based on the value of request, send stuff the the right address
    // ideally, we push the argument onto a caller's stack, and the kernel simply read from user's stack based on the command
    if(request == HandlerCode::CREATE) {

    } else if(request == HandlerCode::MY_TID) {

    } 
} 

void Kernel::allocate_new_task(int parent_id, int priority, void (*pc)()) {
    if (p_id_counter < 30) {
        tasks[p_id_counter] = new (task_slab_address) TaskDescriptor(p_id_counter, parent_id, priority, pc); 
        task_slab_address += sizeof(TaskDescriptor);
        p_id_counter += 1;
    } else {
        char m1[] = "out of task space\r\n";
        uart_puts(0, 0, m1, sizeof(m1) - 1);
    }
}

 
// int Kernel::Create(int priority, void (*function)()) {
//     // scheduler.add_task(0, p_id_counter);
//     // tasks[p_id_counter] = TaskDescriptor(p_id_counter, MyTid(), 0);
//     // p_id_counter += 1; 
//     return 1;
// }

// int Kernel::MyTid() {
//     return active_task->task_id;
// }

// int Kernel::MyParentTid() {
//     return active_task->parent_id;
// }

// void Kernel::Yield() {
//     // scheduler.add_task(active_task->priority, active_task->task_id); // will work later
//     active_task = &tasks[scheduler.get_next()];
// }

// void Exit() {
//     // doesn't need to do anything at the moment until scheduler mature
// }
