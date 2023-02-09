#include "clock.h"

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
static const int TIMER_INTERRUPT_ID = 97; // base timer interrupt is 96, +1 for C1

uint32_t Clock::clo(void) {
	return timer->CLO;
}

uint32_t Clock::chi(void) {
	return timer->CHI;
}

uint64_t Clock::time(void) {
	return ((uint64_t)timer->CHI << 32) | (uint64_t)timer->CLO;
}

void Clock::set_comparator(uint32_t interrupt_time, uint32_t reg_num) {
#ifdef OUR_DEBUG
	if (reg_num > 3) {
		return;
	}
#endif
#ifdef OUR_DEBUG
	if (timer->CS & (1 << reg_num)) {
#endif
		// Clear the match detect status bit
		timer->CS |= 1 << reg_num;
#ifdef OUR_DEBUu
	}
#endif

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

void Clock::enable_clock_one_interrupts(void) {
	Interrupt::enable_interrupt_for(TIMER_INTERRUPT_ID);
}
