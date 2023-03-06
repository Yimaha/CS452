#include "clock.h"
#include "../server/terminal_admin.h"
#include "../utils/printf.h"

struct TIMER {
	uint32_t CS;  // System Timer Control/Status
	uint32_t CLO; // System Timer Counter Lower 32 bits
	uint32_t CHI; // System Timer Counter Higher 32 bits
	uint32_t C0;  // System Timer Compare 0
	uint32_t C1;  // System Timer Compare 1
	uint32_t C2;  // System Timer Compare 2
	uint32_t C3;  // System Timer Compare 3
};

static char* const TIMER_BASE = (char*)(0xFE000000 + 0x3000);
static volatile TIMER* const timer = (TIMER*)(TIMER_BASE);

uint32_t Clock::clo() {
	return timer->CLO;
}

uint32_t Clock::chi() {
	return timer->CHI;
}

// Fetches the current system time in microseconds, counting from power on.
uint64_t Clock::system_time() {
	return ((uint64_t)timer->CHI << 32) | (uint64_t)timer->CLO;
}

void Clock::enable_clock_one_interrupts() {
	Interrupt::enable_interrupt_for(TIMER_INTERRUPT_ID);
}

Clock::TimeKeeper::TimeKeeper() {
	if (last_ping == 0) {
		last_ping = Clock::system_time();
	}
}

Clock::TimeKeeper::~TimeKeeper() { }

void Clock::TimeKeeper::start() {
	tick_tracker = system_time() + MICROS_PER_TICK;
	set_comparator(tick_tracker);
}

void Clock::TimeKeeper::tick() {
	tick_tracker += MICROS_PER_TICK;
	set_comparator(tick_tracker);
}

uint64_t Clock::TimeKeeper::get_idle_time() {
	return idle_time;
}

uint64_t Clock::TimeKeeper::get_total_time() {
	return total_time;
}

void Clock::TimeKeeper::idle_start() {
	last_idle_ping = system_time();
}

void Clock::TimeKeeper::idle_end() {
	uint64_t t = system_time();
	idle_time += t - last_idle_ping;
}

void Clock::TimeKeeper::update_total_time() {
	uint64_t t = system_time();

	total_time += t - last_ping;
	last_ping = t;
}

void Clock::TimeKeeper::set_comparator(uint32_t interrupt_time, uint32_t reg_num) {
#ifdef OUR_DEBUG
	if (reg_num > 3) {
		return;
	}
#endif
	// Clear the match detect status bit
	timer->CS |= 1 << reg_num;

	if (reg_num == 0) {
		timer->C0 = interrupt_time;
	} else if (reg_num == 1) {
		timer->C1 = interrupt_time;
	} else if (reg_num == 2) {
		timer->C2 = interrupt_time;
	} else if (reg_num == 3) {
		timer->C3 = interrupt_time;
	}
}
