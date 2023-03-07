
#pragma once
#include <stdint.h>

extern "C" void interrupt_handler();
extern "C" uint64_t read_esr();