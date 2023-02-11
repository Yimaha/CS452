#pragma once
#include "interrupt.h"
#include <stdint.h>

namespace Clock
{
const int TIMER_INTERRUPT_ID = 97; // base timer interrupt is 96, +1 for C1
const int MICROS_PER_TICK = 10000; // 10ms per tick

uint32_t clo();
uint32_t chi();
uint64_t time();

void set_comparator(uint32_t interrupt_time, uint32_t reg_num = 1);
void clear_cs(uint32_t reg_num = 1);
void enable_clock_one_interrupts();
}