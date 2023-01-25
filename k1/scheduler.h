
#pragma once

#define NUM_PRIORITIES 3
#include "buffer.h"
#include "rpi.h"
#include "utility.h"

const static int NO_TASKS = -1;

class Scheduler
{
public:
    Scheduler();
    int get_next();
    void add_task(int priority, int task_id);

private:
    RingBuffer<int> ready_queue[NUM_PRIORITIES];
};
