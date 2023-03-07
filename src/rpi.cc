#include "rpi.h"
#include "utils/printf.h"
#include "interrupt_handler.h"


/*************** GPIO ***************/

static const uint32_t GPIO_INPUT = 0x00;
static const uint32_t GPIO_OUTPUT = 0x01;
static const uint32_t GPIO_ALTFN0 = 0x04;
static const uint32_t GPIO_ALTFN1 = 0x05;
static const uint32_t GPIO_ALTFN2 = 0x06;
static const uint32_t GPIO_ALTFN3 = 0x07;
static const uint32_t GPIO_ALTFN4 = 0x03;
static const uint32_t GPIO_ALTFN5 = 0x02;

static const uint32_t GPIO_NONE = 0x00;
static const uint32_t GPIO_PUP = 0x01;
static const uint32_t GPIO_PDP = 0x02;

static void setup_gpio(uint32_t pin, uint32_t setting, uint32_t resistor) {
	uint32_t reg = pin / 10;
	uint32_t shift = (pin % 10) * 3;
	uint32_t status = gpio->GPFSEL[reg]; // read status
	status &= ~(7u << shift);			 // clear bits
	status |= (setting << shift);		 // set bits
	gpio->GPFSEL[reg] = status;			 // write back

	reg = pin / 16;
	shift = (pin % 16) * 2;
	status = gpio->PUP_PDN_CNTRL_REG[reg]; // read status
	status &= ~(3u << shift);			   // clear bits
	status |= (resistor << shift);		   // set bits
	gpio->PUP_PDN_CNTRL_REG[reg] = status; // write back
}

void init_gpio() {
	setup_gpio(18, GPIO_ALTFN4, GPIO_NONE);
	setup_gpio(19, GPIO_ALTFN4, GPIO_NONE);
	setup_gpio(20, GPIO_ALTFN4, GPIO_NONE);
	setup_gpio(21, GPIO_ALTFN4, GPIO_NONE);
	setup_gpio(24, GPIO_INPUT, GPIO_NONE);
}

static const uint32_t SPI_CNTL0_DOUT_HOLD_SHIFT = 12;
static const uint32_t SPI_CNTL0_CS_SHIFT = 17;
static const uint32_t SPI_CNTL0_SPEED_SHIFT = 20;

static const uint32_t SPI_CNTL0_POSTINPUT = 0x00010000;
static const uint32_t SPI_CNTL0_VAR_CS = 0x00008000;
static const uint32_t SPI_CNTL0_VAR_WIDTH = 0x00004000;
static const uint32_t SPI_CNTL0_Enable = 0x00000800;
static const uint32_t SPI_CNTL0_In_Rising = 0x00000400;
static const uint32_t SPI_CNTL0_Clear_FIFOs = 0x00000200;
static const uint32_t SPI_CNTL0_Out_Rising = 0x00000100;
static const uint32_t SPI_CNTL0_Invert_CLK = 0x00000080;
static const uint32_t SPI_CNTL0_SO_MSB_FST = 0x00000040;
static const uint32_t SPI_CNTL0_MAX_SHIFT = 0x0000003F;

static const uint32_t SPI_CNTL1_CS_HIGH_SHIFT = 8;

static const uint32_t SPI_CNTL1_Keep_Input = 0x00000001;
static const uint32_t SPI_CNTL1_SI_MSB_FST = 0x00000002;
static const uint32_t SPI_CNTL1_Done_IRQ = 0x00000040;
static const uint32_t SPI_CNTL1_TX_EM_IRQ = 0x00000080;

static const uint32_t SPI_STAT_TX_FIFO_MASK = 0xFF000000;
static const uint32_t SPI_STAT_RX_FIFO_MASK = 0x00FF0000;
static const uint32_t SPI_STAT_TX_FULL = 0x00000400;
static const uint32_t SPI_STAT_TX_EMPTY = 0x00000200;
static const uint32_t SPI_STAT_RX_FULL = 0x00000100;
static const uint32_t SPI_STAT_RX_EMPTY = 0x00000080;
static const uint32_t SPI_STAT_BUSY = 0x00000040;
static const uint32_t SPI_STAT_BIT_CNT_MASK = 0x0000003F;

void init_spi(uint32_t channel) {
	uint32_t reg = aux->ENABLES;
	reg |= (2 << channel);
	aux->ENABLES = reg;
	spi[channel]->CNTL0 = SPI_CNTL0_Clear_FIFOs;
	uint32_t speed = (700000000 / (2 * 0x400000)) - 1; // for maximum bitrate 0x400000
	spi[channel]->CNTL0 = (speed << SPI_CNTL0_SPEED_SHIFT) | SPI_CNTL0_VAR_WIDTH | SPI_CNTL0_Enable | SPI_CNTL0_In_Rising | SPI_CNTL0_SO_MSB_FST;
	spi[channel]->CNTL1 = SPI_CNTL1_SI_MSB_FST;
}

static void spi_send_recv(uint32_t channel, const char* sendbuf, size_t sendlen, char* recvbuf, size_t recvlen) {
	size_t sendidx = 0;
	size_t recvidx = 0;
	while (sendidx < sendlen || recvidx < recvlen) {
		uint32_t data = 0;
		size_t count = 0;

		// prepare write data
		for (; sendidx < sendlen && count < 24; sendidx += 1, count += 8) {
			data |= (sendbuf[sendidx] << (16 - count));
		}
		data |= (count << 24);

		// always need to write something, otherwise no receive
		while (spi[channel]->STAT & SPI_STAT_TX_FULL)
			asm volatile("yield");
		if (sendidx < sendlen) {
			spi[channel]->TXHOLD_REGa = data; // keep chip-select active, more to come
		} else {
			spi[channel]->IO_REGa = data;
		}

		// read transaction
		while (spi[channel]->STAT & SPI_STAT_RX_EMPTY)
			asm volatile("yield");
		data = spi[channel]->IO_REGa;

		// process data, if needed, assume same byte count in transaction
		size_t max = (recvlen - recvidx) * 8;
		if (count > max)
			count = max;
		for (; count > 0; recvidx += 1, count -= 8) {
			recvbuf[recvidx] = (data >> (count - 8)) & 0xFF;
		}
	}
}

static char uart_read_register(size_t spiChannel, size_t uartChannel, char reg) {
	char req[2] = { 0 };
	char res[2] = { 0 };
	req[0] = (uartChannel << UART_CHANNEL_SHIFT) | (reg << UART_ADDR_SHIFT) | UART_READ_ENABLE;
	spi_send_recv(spiChannel, req, 2, res, 2);
	return res[1];
}

static void uart_write_register(size_t spiChannel, size_t uartChannel, char reg, char data) {
	char req[2] = { 0 };
	req[0] = (uartChannel << UART_CHANNEL_SHIFT) | (reg << UART_ADDR_SHIFT);
	req[1] = data;
	spi_send_recv(spiChannel, req, 2, NULL, 0);
}


static void uart_init_channel(size_t spiChannel, size_t uartChannel, size_t baudRate)
{
  // set baud rate
  uart_write_register(spiChannel, uartChannel, UART_LCR, UART_LCR_DIV_LATCH_EN);
  uint32_t bauddiv = 14745600 / (baudRate * 16);
  uart_write_register(spiChannel, uartChannel, UART_DLH, (bauddiv & 0xFF00) >> 8);
  uart_write_register(spiChannel, uartChannel, UART_DLL, (bauddiv & 0x00FF));

  // set serial byte configuration: 8 bit, no parity, 1 stop bit
  uart_write_register(spiChannel, uartChannel, UART_LCR, 0x3);

  // clear and enable fifos,u(wait since clearing fifos takes time)
  uart_write_register(spiChannel, uartChannel, UART_FCR, UART_FCR_RX_FIFO_RESET | UART_FCR_TX_FIFO_RESET | UART_FCR_FIFO_EN);
  for (int i = 0; i < 65535; ++i)
    asm volatile("yield");
}

static void uart_init_channel_train(size_t spiChannel, size_t uartChannel, size_t baudRate)
{
  // set baud rate
  uart_write_register(spiChannel, uartChannel, UART_LCR, UART_LCR_DIV_LATCH_EN);
  uint32_t bauddiv = 14745600 / (baudRate * 16);
  uart_write_register(spiChannel, uartChannel, UART_DLH, (bauddiv & 0xFF00) >> 8);
  uart_write_register(spiChannel, uartChannel, UART_DLL, (bauddiv & 0x00FF));

  // set serial byte configuration: 8 bit, no parity, 2 stop bit
  uart_write_register(spiChannel, uartChannel, UART_LCR, 0x7);

  // clear and enable fifos,u(wait since clearing fifos takes time)
  uart_write_register(spiChannel, uartChannel, UART_FCR, UART_FCR_RX_FIFO_RESET | UART_FCR_TX_FIFO_RESET | UART_FCR_FIFO_EN);
  for (int i = 0; i < 65535; ++i)
    asm volatile("yield");
}

void init_uart(uint32_t spiChannel) {
	uart_write_register(spiChannel, 0, UART_IOControl, UART_IOControl_RESET); // resets both channels
	uart_init_channel(spiChannel, 0, 115200);
	uart_init_channel_train(spiChannel, 1, 2400);
}

char uart_getc(size_t spiChannel, size_t uartChannel) {
	while (uart_read_register(spiChannel, uartChannel, UART_RXLVL) == 0)
		asm volatile("yield");
	return uart_read_register(spiChannel, uartChannel, UART_RHR);
}

void uart_putc(size_t spiChannel, size_t uartChannel, char c) {
	while (uart_read_register(spiChannel, uartChannel, UART_TXLVL) == 0)
		asm volatile("yield");
	uart_write_register(spiChannel, uartChannel, UART_THR, c);
}

bool uart_putc_non_blocking(size_t spiChannel, size_t uartChannel, char c) {
	if (uart_read_register(spiChannel, uartChannel, UART_TXLVL) == 0) {
		return false;
	}	
	uart_write_register(spiChannel, uartChannel, UART_THR, c);
	return true;
}


void uart_puts(size_t spiChannel, size_t uartChannel, const char* buf, size_t blen) {
	static const size_t max = 32;
	char temp[max];
	temp[0] = (uartChannel << UART_CHANNEL_SHIFT) | (UART_THR << UART_ADDR_SHIFT);
	size_t tlen = uart_read_register(spiChannel, uartChannel, UART_TXLVL);
	if (tlen > max)
		tlen = max;
	for (size_t bidx = 0, tidx = 1;;) {
		if (tidx < tlen && bidx < blen) {
			temp[tidx] = buf[bidx];
			bidx += 1;
			tidx += 1;
		} else {
			spi_send_recv(spiChannel, temp, tidx, NULL, 0);
			if (bidx == blen)
				break;
			tlen = uart_read_register(spiChannel, uartChannel, UART_TXLVL);
			if (tlen > max)
				tlen = max;
			tidx = 1;
		}
	}
}

void uart_put(size_t spiChannel, size_t uartChannel, char reg, char data) {
	uart_write_register(spiChannel, uartChannel, reg, data);
}


char uart_get(size_t spiChannel, size_t uartChannel, char reg) {
	return uart_read_register(spiChannel, uartChannel, reg);
}

bool uart_getc_non_blocking(size_t spiChannel, size_t uartChannel, char* c) {
	if (uart_read_register(spiChannel, uartChannel, UART_RXLVL) == 0) {
		return false;
	}
	*c = uart_read_register(spiChannel, uartChannel, UART_RHR);
	return true;
}

int uart_get_all(size_t spiChannel, size_t uartChannel, char* buffer) {
	int i = 0;
	while (uart_read_register(spiChannel, uartChannel, UART_RXLVL) != 0){
		buffer[i] = uart_read_register(spiChannel, uartChannel, UART_RHR);
		i+=1;
	}
	return i;
}

// define our own memset to avoid SIMD instructions emitted from the compiler
extern "C" void* memset(void* s, int c, size_t n) {
	for (char* it = (char*)s; n > 0; --n)
		*it++ = c;
	return s;
}

// define our own memcpy to avoid SIMD instructions emitted from the compiler
extern "C" void* memcpy(void* __restrict__ dest, const void* __restrict__ src, size_t n) {
	char* sit = (char*)src;
	char* cdest = (char*)dest;
	for (size_t i = 0; i < n; ++i)
		*(cdest++) = *(sit++);
	return dest;
}

/********** Utility Functions **********/

extern "C" void val_print(uint64_t c) {
	// print out each byte as a char
	char buf[8];
	for (int i = 0; i < 8; ++i) {
		buf[7 - i] = (c >> (i * 8)) & 0xFF;
	}

	uart_puts(0, 0, buf, 8);
}

extern "C" void print_interrupt() {
	uart_puts(0, 0, "interrupt!\r\n", 12);
	while (1) {
	};
}

extern "C" void print_exception_arg(uint64_t arg) {
	printf("exception arg: %x\r\n", arg);
	uint64_t error_code = (read_esr() >> 26) & 0x3f;
	printf("ESR: %llx\r\n", error_code);
	while (1) {
	};
}

extern "C" void print_hex_arg(uint64_t arg) {
	printf("arg: %x\r\n", arg);
}

// Crash the system
extern "C" void restart(void) {
	asm volatile("b reboot");
}

// Function that crashes the system
// Called when an assert fails
extern "C" void assert_crash(const char* msg) {

	if (msg != nullptr) {
		printf(msg);
	}

	char fail[] = "assertion failed\r\n";
	printf(fail);

	// Wait a bit
	for (int i = 0; i < 100000; ++i)
		asm volatile("yield");

	// Crash the system
	asm volatile("b reboot");
}

// Assert function
// If cond is false, the system crashes
// Also takes an optional message to print
// (will only print first `len` chars of message)
void kernel_assert(const bool cond, const char* msg) {
	if (!cond)
		assert_crash(msg);
}


