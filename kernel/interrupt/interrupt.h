#pragma once
#include "clock.h"
#include <stdint.h>
static char* const GIC_BASE = (char*)0xFF840000;

namespace Interrupt
{
void init_interrupt();
void enable_interrupt_for(uint32_t id);
}
