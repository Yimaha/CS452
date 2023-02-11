#pragma once
#include "interrupt.h"
#include <stdint.h>

namespace Clock
{
const int TIMER_INTERRUPT_ID = 97; // base timer interrupt is 96, +1 for C1
const int MICROS_PER_TICK = 10000; // 10ms per tick

// Timer Functions
uint32_t clo();
uint32_t chi();
uint64_t time();

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
void clear_cs(uint32_t reg_num = 1);
void enable_clock_one_interrupts();
}