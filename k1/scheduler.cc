#include "scheduler.h"

Scheduler::Scheduler() {}

int Scheduler::get_next()
{
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        if (!ready_queue[i].is_empty())
        {
            return ready_queue[i].pop_front();
        }
    }

    return NO_TASKS; // no tasks to run
}

void Scheduler::add_task(int priority, int task_id)
{
    ready_queue[priority].push_back(task_id);
}
