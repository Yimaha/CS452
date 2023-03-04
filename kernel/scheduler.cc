#include "scheduler.h"

using namespace Task;

Scheduler::Scheduler() { }

int Scheduler::get_next() {
	for (int i = 0; i < NUM_PRIORITIES; i++) {
		if (!ready_queue[i].empty()) {
			int id = ready_queue[i].front();
			ready_queue[i].pop();
			return id;
		}
	}

	return NO_TASKS; // no tasks to run
}

void Scheduler::add_task(Priority priority, int task_id) {
	ready_queue[static_cast<int>(priority)].push(task_id);
}
