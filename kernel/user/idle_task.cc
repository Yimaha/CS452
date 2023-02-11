#include "idle_task.h"
#include "../server/clock_server.h"
#include "../utils/printf.h"

#define IDLE_BUFSIZE 30

void SystemTask::idle_task() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	char buf[IDLE_BUFSIZE];
	while (true) {
		sprintf(buf, "Current ticks: %d\r\n", Clock::Time(clock_tid));
		Task::KernelPrint(buf);
		for (int i = 0; i < 300000; ++i)
			asm volatile("yield");
	}
}

void SystemTask::delay_task() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int my_tid = Task::MyTid();
	char buf[IDLE_BUFSIZE];
	int delay = 1000;
	while (true) {
		Clock::Delay(clock_tid, delay);
		sprintf(buf, "T%d delayed %d ticks\r\n", my_tid, delay);
		Task::KernelPrint(buf);
	}
}

void SystemTask::delay_task_2() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int my_tid = Task::MyTid();
	char buf[IDLE_BUFSIZE];
	int delay = 256;
	while (true) {
		Clock::Delay(clock_tid, delay);
		sprintf(buf, "T%d delayed %d ticks\r\n", my_tid, delay);
		Task::KernelPrint(buf);
	}
}

void SystemTask::delay_until_task() {
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int my_tid = Task::MyTid();
	char buf[IDLE_BUFSIZE];
	int delay = 314;
	while (true) {
		sprintf(buf, "T%d delayed until %d ticks\r\n", my_tid, delay);
		Task::KernelPrint(buf);
		Clock::DelayUntil(clock_tid, delay);
		delay += 314;
	}
}