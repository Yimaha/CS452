#pragma once
#include "../rpi.h"
#include "interrupt.h"
#include <stdint.h>

namespace UART
{
const int UART_INTERRUPT_ID = 145;
const int UART_RX_TIMEOUT = 12;
const int UART_TXR_INTERRUPT = 2;
const int UART_CLEAR = 1; // no more uart interrupt exists
const char TRANS_ENABLE_BIT = (char)0b10;
const char RECEIVE_ENABLE_BIT = (char)0b01;
void enable_uart_interrupt();
void clear_uart_interrupt();
char get_control_bits(bool enable_trans, bool enable_receive);
}