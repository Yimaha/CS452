
#pragma once

#define NUM_PRIORITIES 4
#include "utils/buffer.h"
#include "utils/utility.h"
#include "rpi.h"

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
