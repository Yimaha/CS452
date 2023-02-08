#include "idle_task.h"

extern "C" void idle_task() {
	while (1) {
		Task::Yield(); // just yield for now
	}
}