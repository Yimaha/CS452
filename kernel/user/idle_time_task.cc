#include "idle_time_task.h"
#include "../kernel.h"

void SystemTask::idle_time_task() {
	// technically idle_task will never need to yield to kernel
	// since there is no context swtich, only interrupt after this point
	int clock_tid = Name::WhoIs(Clock::CLOCK_SERVER_NAME);
	int terminal_tid = Name::WhoIs(Terminal::TERMINAL_ADMIN);
	uint64_t idle_time, total_time;
	char buf[100];

	while (true) {
		Clock::IdleStats(&idle_time, &total_time);
		uint64_t leading = idle_time * 100 / total_time;
		uint64_t trailing = (idle_time * 100000) / total_time % 1000;
		Terminal::Puts(terminal_tid, Terminal::SAVE_CURSOR);
		// Clock::Delay(clock_tid, 1);
		sprintf(buf, "\033[1;80HIdle time: %llu", idle_time);
		Terminal::Puts(terminal_tid, buf);
		// Clock::Delay(clock_tid, 1);
		sprintf(buf, "\033[2;80HTotal time: %llu", total_time);
		Terminal::Puts(terminal_tid, buf);
		// Clock::Delay(clock_tid, 1);
		sprintf(buf, "\033[3;80HPercent: %llu.%03llu", leading, trailing);
		Terminal::Puts(terminal_tid, buf);
		// Clock::Delay(clock_tid, 1);
		Terminal::Puts(terminal_tid, Terminal::RESTORE_CURSOR);

		Clock::Delay(clock_tid, 200);
	}
}
