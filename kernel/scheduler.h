
#pragma once

#include "etl/queue.h"
#include "rpi.h"
#include "utils/buffer.h"
#include "utils/utility.h"

enum class Priority {
	LAUNCH_PRIORITY,
	CRITICAL_PRIORITY,
	HIGH_PRIORITY,
	TERMINAL_PRIORITY,
	IDLE_PRIORITY,
};

namespace Task
{
const static int NO_TASKS = -1;
const static int NUM_PRIORITIES = static_cast<int>(Priority::IDLE_PRIORITY) + 1;
const static int SCHEDULER_QUEUE_SIZE = 128; // I can't believe i am saying this but it seems like we are running out
class Scheduler {
public:
	Scheduler();
	int get_next();
	void add_task(Priority priority, int task_id);

private:
	etl::queue<int, SCHEDULER_QUEUE_SIZE> ready_queue[NUM_PRIORITIES];
};
}