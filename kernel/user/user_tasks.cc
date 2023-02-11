#include "user_tasks.h"
#include "idle_task.h"

void UserTask::first_user_task() {
	while (true) {
		// Create the name server
		printf("creating name server\r\n");
		Task::Create(2, &Name::name_server);

		// Create the clock server
		printf("creating clock server\r\n");
		Task::Create(1, &Clock::clock_server);

		// Create the clock notifier
		printf("creating clock notifier\r\n");
		Task::Create(1, &Clock::clock_notifier);

		// Set the idle tasks running
		printf("creating idle task\r\n");
		Task::Create(3, &SystemTask::idle_task);

		printf("creating delay task\r\n");
		Task::Create(3, &SystemTask::delay_task);

		printf("creating delay task 2\r\n");
		Task::Create(3, &SystemTask::delay_task_2);

		printf("creating delay until task\r\n");
		Task::Create(3, &SystemTask::delay_until_task);

		printf("exiting first user task\r\n");
		Task::Exit();
	}
}

void UserTask::Task_test_0() {
	while (1) {
		char msg[] = "user task 0\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
		Task::Exit();
	}
}

void UserTask::Task_test_1() {
	while (1) {
		char msg[] = "user task 1\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}

void UserTask::Task_test_2() {
	while (1) {
		char msg[] = "user task 2\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}

void UserTask::Task_test_3() {
	while (1) {
		char msg[] = "user task 3\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
		Task::Exit();
	}
}

void UserTask::Task_test_4() {
	while (1) {
		char msg[] = "user task 4\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}