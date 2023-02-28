
#include "uart.h"

using namespace UART;

void UART::enable_uart_interrupt() {
    // allow pin 24 to start interrupting us
	gpio->GPLEN0 = 1 << 24;

	// uart_put(0, 1, UART_IER, get_control_bits(false, false, true));
	Interrupt::enable_interrupt_for(UART_INTERRUPT_ID);
}

void UART::clear_uart_0_interrupt() {
	// write to bit 24 and inform we are good with all current interrupt! you can disable it
	gpio->GPEDS0 = 1 << 24;
}

void UART::clear_uart_1_interrupt() {
	// write to bit 24 and inform we are good with all current interrupt! you can disable it
	gpio->GPEDS1 = 1 << 24;
}

char UART::get_control_bits(bool enable_trans, bool enable_receive, bool enable_CTS) {
	return (enable_trans ? TRANS_ENABLE_BIT : 0x0) | (enable_receive ? RECEIVE_ENABLE_BIT : 0x0) | (enable_CTS ? 0b10001000 : 0x0);
}