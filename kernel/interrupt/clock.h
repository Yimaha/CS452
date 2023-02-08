#pragma once
#include "interrupt.h"
#include <stdint.h>

namespace Clock
{
static const int TIMER_INTERRUPT_ID = 97; // base timer interrupt is 96, +1 for C1

uint32_t clo();
uint32_t chi();
uint64_t time();

void set_comparator(uint32_t interrupt_time, uint32_t reg_num = 1);
void enable_clock_one_interrupts();
}