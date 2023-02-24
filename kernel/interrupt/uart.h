#pragma once
#include "../rpi.h"
#include "interrupt.h"
#include <stdint.h>

namespace UART
{
const int UART_INTERRUPT_ID = 145;
const int UART_RX_TIMEOUT = 12;
void enable_uart_interrupt();
}