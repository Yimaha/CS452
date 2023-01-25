#pragma once

#include <stdint.h>
#include <stddef.h>

void init_gpio();
void init_spi(uint32_t channel);
void init_uart(uint32_t spiChannel);
char uart_getc(size_t spiChannel, size_t uartChannel);
void uart_putc(size_t spiChannel, size_t uartChannel, char c);
void uart_puts(size_t spiChannel, size_t uartChannel, const char *buf, size_t blen);

// anything that can be invoked from assembly and is useful goes here
extern "C" void *memset(void *s, int c, size_t n);
extern "C" void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n);
extern "C" void val_print(uint64_t c);
extern "C" void print_exception();
extern "C" void assert_crash(const char *msg = nullptr, const size_t len = 0);
void kernel_assert(bool cond, const char *msg = nullptr, const size_t len = 0);
