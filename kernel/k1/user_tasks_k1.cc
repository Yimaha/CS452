#include "user_tasks_k1.h"

void helper_0(Priority priority) {
	// create a task and print the creation result
	int task_id = Task::Create(priority, &UserTask::Sub_Task);
	char msg[] = "Created: task ";
	uart_puts(0, 0, msg, sizeof(msg) - 1);
	print_int(task_id);
	print("\r\n", 2);
}

void helper_sub(int id, int p_id) {
	// print parent and task id
	print("my task id: ", 12);
	print_int(id);
	print("; my parent id: ", 16);
	print_int(p_id);
	print("\r\n", 2);
}

extern "C" void UserTask::Task_0() {
	while (true) {
		char msg[] = "entered into user task 0\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		helper_0(Priority::HIGH_PRIORITY);
		helper_0(Priority::HIGH_PRIORITY);
		helper_0(Priority::LAUNCH_PRIORITY);
		helper_0(Priority::LAUNCH_PRIORITY);

		char msg5[] = "exiting task 0\r\n";
		uart_puts(0, 0, msg5, sizeof(msg5) - 1);
		Task::Exit();
	}
}

extern "C" void UserTask::Sub_Task() {
	int id = Task::MyTid();
	int p_id = Task::MyParentTid();

	helper_sub(id, p_id);
	Task::Yield();
	helper_sub(id, p_id);

	Task::Exit();
}
