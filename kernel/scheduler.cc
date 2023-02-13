#include "scheduler.h"

using namespace Task;

Scheduler::Scheduler() { }

int Scheduler::len() {
	int len = 0;
	for (int i = 0; i < NUM_PRIORITIES; i++) {
		len += ready_queue[i].size();
	}
	return len;
}

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

int Scheduler::peek_next() {
	for (int i = 0; i < NUM_PRIORITIES; i++) {
		if (!ready_queue[i].empty()) {
			return ready_queue[i].front();
		}
	}

	return NO_TASKS; // no tasks to run
}

void Scheduler::add_task(int priority, int task_id) {
	ready_queue[priority].push(task_id);
}
