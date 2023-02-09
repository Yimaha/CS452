#pragma once
#include "interrupt.h"
namespace Clock
{
uint32_t clo(void);
uint32_t chi(void);
uint64_t time(void);
void set_comparator(uint32_t interrupt_time, uint32_t reg_num = 1);
void enable_clock_one_interrupts(void);
}