#include "user_tasks.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "../k3/k3_client.h"
#include "../server/uart_server.h"

void UserTask::first_user_task() {
	while (true) {
		// Create the name server
		printf("creating name server\r\n");
		Task::Create(2, &Name::name_server);

		// Register in the name server
		printf("registering first user task\r\n");
		Name::RegisterAs(UserTask::FIRST_USER_TASK_NAME);

		// Create the clock server
		printf("creating clock server\r\n");
		Task::Create(1, &Clock::clock_server);

		// Create the clock notifier
		printf("creating clock notifier\r\n");
		Task::Create(1, &Clock::clock_notifier);

		printf("creating idle task\r\n");
		Task::Create(7, &SystemTask::idle_task);

		// at the moment, only support uart0
		Task::Create(1, &UART::uart_server);

		// Set the client tasks running
		printf("creating client tasks\r\n");
		for (int i = 3; i < 7; ++i) {
			Task::Create(i, &SystemTask::k3_client_task);
		}

		const char c1_params[] = { 10, 20 };
		const char c2_params[] = { 23, 9 };
		const char c3_params[] = { 33, 6 };
		const char c4_params[] = { 71, 3 };
		const char* client_params[] = { c1_params, c2_params, c3_params, c4_params };

		// Receieve
		for (int i = 0; i < 4; ++i) {
			int sender_tid;
			Message::Receive::Receive(&sender_tid, nullptr, 0);
			Message::Reply::Reply(sender_tid, client_params[i], 2);
		}

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