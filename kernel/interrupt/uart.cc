
#include "uart.h"

using namespace UART;

void UART::enable_uart_interrupt() {
    // allow pin 24 to start interrupting us
	gpio->GPLEN0 = 1 << 24;
	uart_put(0, 0, UART_IER, 0b01);
	Interrupt::enable_interrupt_for(UART_INTERRUPT_ID);
}
