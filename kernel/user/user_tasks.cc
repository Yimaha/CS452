#include "user_tasks.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "../k3/k3_client.h"
#include "../server/uart_server.h"
#include "../k4/k4_client.h"
void UserTask::first_user_task() {
	while (true) {
		// Create the name server
		Task::Create(2, &Name::name_server);

		// Register in the name server
		Name::RegisterAs(UserTask::FIRST_USER_TASK_NAME);

		// Create the clock server
		Task::Create(1, &Clock::clock_server);

		// Create the clock notifier
		Task::Create(1, &Clock::clock_notifier);

		Task::Create(7, &SystemTask::idle_task);

		// at the moment, only support uart0
		Task::Create(1, &UART::uart_server);
		// // printf("exiting first user task\r\n");
		Task::Create(2, &SystemTask::k4_dummy); // temp dummy test to see if io works
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