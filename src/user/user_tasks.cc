#include "user_tasks.h"
#include "../k3/k3_client.h"
#include "../k4/k4_client.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../server/global_pathing_server.h"
#include "../server/local_pathing_server.h"
#include "../server/train_admin.h"
#include "../server/uart_server.h"
#include "../server/track_server.h"
#include "../tc1/tc1_client.h"
#include "../utils/utility.h"

void UserTask::launch() {
	while (true) {
		// Create the name server
		// note that it is idle priority because it will force everyone to register their name first.
		// and asking for name will also delay you, forcing you to complete them during start up.
		Task::Create(Priority::CRITICAL_PRIORITY, &Name::name_server);

		// Register in the name server
		Name::RegisterAs(UserTask::LAUNCH_TASK_NAME);

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

		Task::Create(Priority::HIGH_PRIORITY, &Planning::global_pathing_server);
		Task::Create(Priority::HIGH_PRIORITY, &Track::track_server);
		Task::Create(Priority::HIGH_PRIORITY, &Train::train_admin);
		Task::Create(Priority::HIGH_PRIORITY, &Sensor::sensor_admin);


		Task::Create(Priority::TERMINAL_PRIORITY, &Terminal::terminal_admin);

		Task::Exit();
	}
}

void UserTask::Task_test_0() {
	while (true) {
		char msg[] = "user task 0\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
		Task::Exit();
	}
}

void UserTask::Task_test_1() {
	while (true) {
		char msg[] = "user task 1\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}

void UserTask::Task_test_2() {
	while (true) {
		char msg[] = "user task 2\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}

void UserTask::Task_test_3() {
	while (true) {
		char msg[] = "user task 3\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
		Task::Exit();
	}
}

void UserTask::Task_test_4() {
	while (true) {
		char msg[] = "user task 4\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		print_int(Task::MyTid());
	}
}