#include "idle_task.h"
#include "../server/clock_server.h"
#include "../utils/printf.h"
#include "user_tasks.h"

#define IDLE_BUFSIZE 64
#define CLIENT_BUFSIZE 2

void SystemTask::k3_client_task() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int my_tid = Task::MyTid();
	int first_user_tid = Name::WhoIs(UserTask::FIRST_USER_TASK_NAME);
	char buf[IDLE_BUFSIZE];
	Message::Send::Send(first_user_tid, nullptr, 0, buf, CLIENT_BUFSIZE);
	int delay = buf[0];
	int repeat = buf[1];
	for (int i = 1; i <= repeat; ++i) {
		Clock::Delay(clock_tid, delay);
		sprintf(buf, "T%d D%d C%d\r\n", my_tid, delay, i);
		Task::KernelPrint(buf);
	}

	Task::Exit();
}

void SystemTask::idle_task() {
	Task::RegisterIdleTid();
	while (true) {
		// asm volatile("wfi");
		Task::Yield();
	}
}
