
#pragma once
#include <stdint.h>
#include "kernel.h"

struct InterruptFrame;


extern "C" void check_sp(); // used to set the current sp, which is kinda useful at the beginning
extern "C" InterruptFrame* first_el0_entry(char * userSP, void (*pc) ());
extern "C" InterruptFrame* to_user(char * userSP, uint64_t results);
extern "C" uint64_t to_kernel(uint64_t exception_code);
extern "C" void handle_syscall();


