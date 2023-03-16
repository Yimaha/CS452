#include "idle_task.h"

void SystemTask::idle_task() {
	// technically idle_task will never need to yield to kernel
	// since there is no context swtich, only interrupt after this point
	while (true) {
		asm volatile("wfi");
	}
}
