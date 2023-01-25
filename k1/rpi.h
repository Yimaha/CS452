#pragma once

#include <stdint.h>
#include <stddef.h>

void init_gpio();
void init_spi(uint32_t channel);
void init_uart(uint32_t spiChannel);
char uart_getc(size_t spiChannel, size_t uartChannel);
void uart_putc(size_t spiChannel, size_t uartChannel, char c);
void uart_puts(size_t spiChannel, size_t uartChannel, const char* buf, size_t blen);
void print_int(uint64_t val);
void print(const char s[], int length);

extern "C" void* memset(void *s, int c, size_t n);
extern "C" void* memcpy(void* __restrict__ dest, const void* __restrict__ src, size_t n);
extern "C" void print(char* c);
extern "C" void val_print(uint64_t c);
extern "C" void print_exception();

