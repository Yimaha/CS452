#include "user_tasks.h"

extern "C" void UserTask::Task_test_0()
{
	while (1)
	{
		char msg[] = "user task 0\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(TaskCreation::MyTid());
		TaskCreation::Exit();
	}
}

extern "C" void UserTask::Task_test_1()
{
	while (1)
	{
		char msg[] = "user task 1\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(TaskCreation::MyTid());
	}
}

extern "C" void UserTask::Task_test_2()
{
	while (1)
	{
		char msg[] = "user task 2\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(TaskCreation::MyTid());
	}
}

extern "C" void UserTask::Task_test_3()
{
	while (1)
	{
		char msg[] = "user task 3\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(TaskCreation::MyTid());
		TaskCreation::Exit();
	}
}

extern "C" void UserTask::Task_test_4()
{
	while (1)
	{
		char msg[] = "user task 4\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(TaskCreation::MyTid());
	}
}