
#pragma once
#include <stdint.h>
#include "kernel.h"

struct InterruptFrame;

extern "C" InterruptFrame *first_el0_entry(char *userSP, void (*pc)());
extern "C" InterruptFrame *to_user(char *userSP, uint64_t results);
extern "C" uint64_t to_kernel(uint64_t exception_code);
extern "C" uint64_t to_kernel_create_tasks(uint64_t exception_code, int priority, void (*function)());
extern "C" void handle_syscall();
