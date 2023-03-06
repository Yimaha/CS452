#pragma once
#include "../rpi.h"
#include "interrupt.h"
#include <stdint.h>

namespace UART
{
const int UART_INTERRUPT_ID = 145;
enum InterruptType {UART_MODEM_INTERRUPT = 0, UART_CLEAR = 1, UART_TXR_INTERRUPT = 2, UART_RX_TIMEOUT = 12, UART_RX_INTERRUPT = 4};
enum InterruptEvents {UART_0_TXR_INTERRUPT, UART_0_RX_TIMEOUT, UART_1_TXR_INTERRUPT, UART_1_RX_INTERRUPT, UART_1_RX_TIMEOUT, UART_1_MSR_INTERRUPT};

const char TRANS_ENABLE_BIT = (char)0b10;
const char RECEIVE_ENABLE_BIT = (char)0b01;
void enable_uart_interrupt();
void clear_uart_interrupt();
char get_control_bits(bool enable_trans, bool enable_receive, bool enable_CTS);
}