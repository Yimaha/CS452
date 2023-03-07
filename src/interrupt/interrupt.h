#pragma once
#include "clock.h"
#include "uart.h"
#include <stdint.h>
static char* const GIC_BASE = (char*)0xFF840000;

namespace Interrupt
{
void init_interrupt();
void enable_interrupt_for(uint32_t id);

// Reads the interrupt id from the GICC_IAR register
uint32_t get_interrupt_id();
bool is_interrupt_clear();

// Writes the interrupt id to the GICC_EOIR register
void end_interrupt(uint32_t id);

}
