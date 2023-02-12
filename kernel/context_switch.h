
#pragma once
#include <stdint.h>
/**
 * A reconstruction of the user stack registers into a readable struct,
 * both by human and machine, and extractable
 * uint64_t's that are stored on the stack.
 *
 * When used, we simply cast addresses given to us into this struct and extract varables accordingly
 * Credit: https://krinkinmu.github.io/2021/01/10/aarch64-interrupt-handling.html
 */
struct InterruptFrame {
	uint64_t x0;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
	uint64_t x5;
	uint64_t x6;
	uint64_t x7;
	uint64_t x8;
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;
	uint64_t x16;
	uint64_t x17;
	uint64_t x18;
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t fp;
	uint64_t lr;
	uint64_t spsr;
	uint64_t data; // used to determine the source of the interrupt, i.e, (0) for synchronous, (1) for IRQ
	uint64_t pc;
};

extern "C" InterruptFrame* first_el0_entry(char* userSP, void (*pc)());
extern "C" InterruptFrame* to_user(uint64_t results, char* userSP, char* userSPSR);
extern "C" InterruptFrame* to_user_interrupted(char* userSP, char* userSPSR, void (*pc)());
extern "C" uint64_t to_kernel(uint64_t exception_code, ...);
extern "C" void handle_syscall();
extern "C" void mmu_registers(char* table_0);

