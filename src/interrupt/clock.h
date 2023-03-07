#pragma once
#include "../user/idle_task.h"
#include "interrupt.h"
#include <stdint.h>

namespace Clock
{
const int TIMER_INTERRUPT_ID = 97; // base timer interrupt is 96, +1 for C1
const int MICROS_PER_TICK = 10000; // 10ms per tick

// Timer Functions
uint32_t clo();
uint32_t chi();
uint64_t system_time();

void enable_clock_one_interrupts();

class TimeKeeper {
public:
	TimeKeeper();
	~TimeKeeper();

	void start();
	void tick();
	uint64_t get_idle_time();
	uint64_t get_total_time();
	void idle_start();
	void idle_end();
	void update_total_time();

private:
	/*
	 * The idea of set_comparator
	 * is that it will set the timer to interrupt at a given time.
	 * Specifically, the timer *should* send out an interrupt
	 * when the least significant 32 bits of the timer (clo)
	 * is equal to the given interrupt_time.
	 *
	 * Regnum specifies which comparator register to use.
	 * By default, we use register C1, as C0 and C2
	 * are used by the GPU.
	 */
	void set_comparator(uint32_t interrupt_time, uint32_t reg_num = 1);
	uint64_t tick_tracker = 0;

	// Time tracking variables
	uint64_t last_ping = 0;
	// Time tracking variables for idle task
	uint64_t last_idle_ping = 0;
	// total time should be kernel + non_idle (roughly)
	uint64_t total_time = 1;
	// idle time should only include time which we are in idle
	uint64_t idle_time = 0;

	// Time since last print. Used to print every 5 seconds
	uint64_t last_print = 0;
};
}