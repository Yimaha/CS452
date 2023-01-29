#include "kernel.h"
#include "rpi.h"

int Create(int priority, void (*function)())
{
    return to_kernel_create_tasks(Kernel::HandlerCode::CREATE, priority, function);
}

void Yield()
{
    to_kernel(Kernel::HandlerCode::YIELD);
}

int MyTid()
{
    return to_kernel(Kernel::HandlerCode::MY_TID);
}

int MyParentTid()
{
    return to_kernel(Kernel::HandlerCode::MY_PARENT_ID);
}

void Exit()
{
    to_kernel(Kernel::HandlerCode::EXIT);
}

Kernel::Kernel()
{
    allocate_new_task(-1, 1, &Task_0);
}

void Kernel::schedule_next_task()
{ // kernel busy waiting on available tasks
    active_task = scheduler.get_next();
    while (active_task == NO_TASKS)
    {
        char m[] = "no tasks available...\r\n";
        uart_puts(0, 0, m, sizeof(m) - 1);
        for (int i = 0; i < 3000000; ++i)
            asm volatile("yield");
        active_task = scheduler.get_next();
    }
}

void Kernel::activate()
{
    if (!tasks[active_task]->initialized)
    {
        tasks[active_task]->initialized = true;

        // startup task, has no parameter or handling
        active_request = first_el0_entry(tasks[active_task]->sp, tasks[active_task]->pc);
    }
    else
    {
        active_request = to_user(tasks[active_task]->sp, tasks[active_task]->prepared_response);
    }
    tasks[active_task]->sp = (char *)active_request;
}

Kernel::~Kernel() {}

void Kernel::handle()
{
    HandlerCode request = (HandlerCode)active_request->x0; // x0 is always the request type

    if (request == HandlerCode::CREATE)
    {
        int priority = active_request->x1;
        void (*user_task)() = (void (*)())active_request->x2;
        tasks[active_task]->prepared_response = p_id_counter;

        // NOTE: allocate_new_task should be called at the end after everything is good
        allocate_new_task(tasks[active_task]->task_id, priority, user_task);
    }
    else if (request == HandlerCode::MY_TID)
    {
        tasks[active_task]->prepared_response = tasks[active_task]->task_id;
    }
    else if (request == HandlerCode::MY_PARENT_ID)
    {
        tasks[active_task]->prepared_response = tasks[active_task]->parent_id;
    }
    else if (request == HandlerCode::YIELD)
    {
        tasks[active_task]->prepared_response = 0x0; // yield doesn't repond anything
    }
    else if (request == HandlerCode::EXIT)
    {
        tasks[active_task]->prepared_response = 0x0;
        tasks[active_task]->kill();
    }
    // we successfully handled the exception, now queue task back if needed
    queue_task(); // this behaviour will probably change in future deliverables
}

void Kernel::queue_task()
{
    if (tasks[active_task]->is_alive())
    { // if it is dead, then longer introduced to the loop
        scheduler.add_task(tasks[active_task]->priority, tasks[active_task]->task_id);
    }
}

// TODO: proper slab allocation
void Kernel::allocate_new_task(int parent_id, int priority, void (*pc)())
{
    if (p_id_counter < 30)
    {
        scheduler.add_task(priority, p_id_counter);
        tasks[p_id_counter] = new (task_slab_address) TaskDescriptor(p_id_counter, parent_id, priority, pc);
        task_slab_address += sizeof(TaskDescriptor);
        p_id_counter += 1;
    }
    else
    {
        char m1[] = "out of task space\r\n";
        uart_puts(0, 0, m1, sizeof(m1) - 1);
    }
}

void Kernel::check_tasks(int task_id)
{
    char m[] = "checking taskDescriptor...\r\n";
    uart_puts(0, 0, m, sizeof(m) - 1);
    tasks[task_id]->show_info();
}
