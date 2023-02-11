#pragma once

#include <stddef.h>
#include <stdint.h>

void init_gpio();
void init_spi(uint32_t channel);
void init_uart(uint32_t spiChannel);
char uart_getc(size_t spiChannel, size_t uartChannel);
void uart_putc(size_t spiChannel, size_t uartChannel, char c);
void uart_puts(size_t spiChannel, size_t uartChannel, const char* buf, size_t blen);

// anything that can be invoked from assembly and is useful goes here
extern "C" void* memset(void* s, int c, size_t n);
extern "C" void* memcpy(void* __restrict__ dest, const void* __restrict__ src, size_t n);

// define our own memcpy to avoid SIMD instructions emitted from the compiler
static inline void* inline_memcpy(void* __restrict__ dest, const void* __restrict__ src, size_t n) {
	char* sit = (char*)src;
	char* cdest = (char*)dest;
	for (size_t i = 0; i < n; ++i)
		*(cdest++) = *(sit++);
	return dest;
}

extern "C" void val_print(uint64_t c);
extern "C" void print_interrupt();
extern "C" void print_exception_arg(uint64_t arg);
extern "C" void print_hex_arg(uint64_t arg);
extern "C" void crash(void);
extern "C" void assert_crash(const char* msg = nullptr);
void kernel_assert(bool cond, const char* msg = nullptr);
