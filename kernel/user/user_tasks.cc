#include "user_tasks.h"
#include "../k3/k3_client.h"
#include "../k4/k4_client.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../server/train_admin.h"
#include "../server/uart_server.h"
#include "../utils/utility.h"

void UserTask::first_user_task() {
	while (true) {
		// Create the name server
		Task::Create(Priority::HIGH_PRIORITY, &Name::name_server);

		// Register in the name server
		Name::RegisterAs(UserTask::FIRST_USER_TASK_NAME);

		// Create the clock server
		Task::Create(Priority::CRITICAL_PRIORITY, &Clock::clock_server);

		// Create the clock notifier
		Task::Create(Priority::CRITICAL_PRIORITY, &Clock::clock_notifier);

		Task::Create(Priority::IDLE_PRIORITY, &SystemTask::idle_task);

		// at the moment, only support uart0
		Task::Create(Priority::CRITICAL_PRIORITY, &UART::uart_0_server_transmit);
		Task::Create(Priority::CRITICAL_PRIORITY, &UART::uart_0_server_receive);
		Task::Create(Priority::CRITICAL_PRIORITY, &UART::uart_1_server_transmit);
		Task::Create(Priority::CRITICAL_PRIORITY, &UART::uart_1_server_receive);

		Task::Create(Priority::HIGH_PRIORITY, &Train::train_admin);
		Task::Create(Priority::HIGH_PRIORITY, &Sensor::sensor_admin);

		// Task::Create(3, &SystemTask::k4_dummy); // temp dummy test to see if io works
		// Task::Create(3, &SystemTask::k4_dummy_train_rev); // temp dummy test to see if io works
		// Task::Create(3, &SystemTask::k4_dummy_train_sensor); // temp dummy test to see if io works
		// Task::Create(3, &SystemTask::k4_dummy_train_switch); // temp dummy test to see if io works

		// Create the terminal admin and user input
		Task::Create(Priority::TERMINAL_PRIORITY, &Terminal::terminal_admin);
		// Task::Create(3, &Courier::terminal_clock_courier);
		// Task::Create(3, &SystemTask::idle_time_task);
		// Task::Create(3, &Courier::sensor_query_courier);
		// Task::Create(3, &Courier::user_input);

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