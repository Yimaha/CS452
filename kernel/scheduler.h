
#pragma once

#include "etl/queue.h"
#include "rpi.h"
#include "utils/buffer.h"
#include "utils/utility.h"

namespace Task
{
const static int NO_TASKS = -1;
const static int NUM_PRIORITIES = 4;
const static int SCHEDULER_QUEUE_SIZE = 64;
class Scheduler {
public:
	Scheduler();
	int get_next();
	void add_task(int priority, int task_id);

private:
	etl::queue<int, SCHEDULER_QUEUE_SIZE> ready_queue[NUM_PRIORITIES];
};

}