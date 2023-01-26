#include "user_tasks_k1.h"

void helper_0(int priority)
{
	// create a task and print the creation result
	int task_id = Create(priority, &Sub_Task);
	char msg[] = "Created: task ";
	uart_puts(0, 0, msg, sizeof(msg) - 1);
	print_int(task_id);
	print("\r\n", 2);
}

void helper_sub(int id, int p_id)
{
	// print parent and task id
	print("my task id: ", 12);
	print_int(id);
	print(" my parent id: ", 15);
	print_int(p_id);
	print("\r\n", 2);
}

extern "C" void Task_0()
{
	while (1)
	{
		char msg[] = "Entered into user task 0\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		helper_0(2);
		helper_0(2);
		helper_0(0);
		helper_0(0);

		char msg5[] = "FirstUserTask: exiting \r\n";
		uart_puts(0, 0, msg5, sizeof(msg5) - 1);
		Exit();
	}
}

extern "C" void Sub_Task()
{
	int id = MyTid();
	int p_id = MyParentTid();

	helper_sub(id, p_id);
	Yield();
	helper_sub(id, p_id);

	Exit();
}
