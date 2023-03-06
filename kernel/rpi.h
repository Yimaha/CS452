#pragma once

#include <stddef.h>
#include <stdint.h>

struct GPIO {
	uint32_t GPFSEL[6];
	uint32_t : 32;
	uint32_t GPSET0;
	uint32_t GPSET1;
	uint32_t : 32;
	uint32_t GPCLR0;
	uint32_t GPCLR1;
	uint32_t : 32;
	uint32_t GPLEV0;
	uint32_t GPLEV1;
	uint32_t : 32;
	uint32_t GPEDS0;
	uint32_t GPEDS1;
	uint32_t : 32;
	uint32_t GPREN0;
	uint32_t GPREN1;
	uint32_t : 32;
	uint32_t GPFEN0;
	uint32_t GPFEN1;
	uint32_t : 32;
	uint32_t GPHEN0;
	uint32_t GPHEN1;
	uint32_t : 32;
	uint32_t GPLEN0;
	uint32_t GPLEN1;
	uint32_t : 32;
	uint32_t GPAREN0;
	uint32_t GPAREN1;
	uint32_t : 32;
	uint32_t GPAFEN0;
	uint32_t GPAFEN1;
	uint32_t _unused[21];
	uint32_t PUP_PDN_CNTRL_REG[4];
};

struct AUX {
	uint32_t IRQ;
	uint32_t ENABLES;
};

struct SPI {
	uint32_t CNTL0; // Control register 0
	uint32_t CNTL1; // Control register 1
	uint32_t STAT;	// Status
	uint32_t PEEK;	// Peek
	uint32_t _unused[4];
	uint32_t IO_REGa;	  // Data
	uint32_t IO_REGb;	  // Data
	uint32_t IO_REGc;	  // Data
	uint32_t IO_REGd;	  // Data
	uint32_t TXHOLD_REGa; // Extended Data
	uint32_t TXHOLD_REGb; // Extended Data
	uint32_t TXHOLD_REGc; // Extended Data
	uint32_t TXHOLD_REGd; // Extended Data
};


static char* const MMIO_BASE = (char*)0xFE000000;
static char* const GPIO_BASE = (char*)(0xFE000000 + 0x200000);
static char* const AUX_BASE = (char*)(0xFE000000 + 0x200000 + 0x15000);

static volatile struct GPIO* const gpio = (struct GPIO*)(GPIO_BASE);
static volatile struct AUX* const aux = (struct AUX*)(AUX_BASE);
static volatile struct SPI* const spi[] = { (struct SPI*)(AUX_BASE + 0x80), (struct SPI*)(AUX_BASE + 0xC0) };

/*************** SPI ***************/

static const char UART_RHR = 0x00;		// R
static const char UART_THR = 0x00;		// W
static const char UART_IER = 0x01;		// R/W
static const char UART_IIR = 0x02;		// R
static const char UART_FCR = 0x02;		// W
static const char UART_LCR = 0x03;		// R/W
static const char UART_MCR = 0x04;		// R/W
static const char UART_LSR = 0x05;		// R
static const char UART_MSR = 0x06;		// R
static const char UART_SPR = 0x07;		// R/W
static const char UART_TXLVL = 0x08;	// R
static const char UART_RXLVL = 0x09;	// R
static const char UART_IODir = 0x0A;	// R/W
static const char UART_IOState = 0x0B;	// R/W
static const char UART_IOIntEna = 0x0C; // R/W
static const char UART_reserved = 0x0D;
static const char UART_IOControl = 0x0E; // R/W
static const char UART_EFCR = 0x0F;		 // R/W

static const char UART_DLL = 0x00; // R/W - only accessible when EFR[4] = 1 and MCR[2] = 1
static const char UART_DLH = 0x01; // R/W - only accessible when EFR[4] = 1 and MCR[2] = 1
static const char UART_EFR = 0x02; // ?   - only accessible when LCR is 0xBF
static const char UART_TCR = 0x06; // R/W - only accessible when EFR[4] = 1 and MCR[2] = 1
static const char UART_TLR = 0x07; // R/W - only accessible when EFR[4] = 1 and MCR[2] = 1

// UART flags
static const char UART_CHANNEL_SHIFT = 1;
static const char UART_ADDR_SHIFT = 3;
static const char UART_READ_ENABLE = 0x80;
static const char UART_FCR_TX_FIFO_RESET = 0x04;
static const char UART_FCR_RX_FIFO_RESET = 0x02;
static const char UART_FCR_FIFO_EN = 0x01;
static const char UART_LCR_DIV_LATCH_EN = 0x80;
static const char UART_EFR_ENABLE_ENHANCED_FNS = 0x10;
static const char UART_IOControl_RESET = 0x08;




void init_gpio();
void init_spi(uint32_t channel);
void init_uart(uint32_t spiChannel);
char uart_getc(size_t spiChannel, size_t uartChannel);
void uart_putc(size_t spiChannel, size_t uartChannel, char c);
bool uart_putc_non_blocking(size_t spiChannel, size_t uartChannel, char c);
bool uart_getc_non_blocking(size_t spiChannel, size_t uartChannel, char* c);

void uart_puts(size_t spiChannel, size_t uartChannel, const char* buf, size_t blen);
void uart_put(size_t spiChannel, size_t uartChannel, char reg, char datas);
char uart_get(size_t spiChannel, size_t uartChannel, char c);
int uart_get_all(size_t spiChannel, size_t uartChannel, char* reg);
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
extern "C" void restart(void);
extern "C" void assert_crash(const char* msg = nullptr);
void kernel_assert(bool cond, const char* msg = nullptr);

