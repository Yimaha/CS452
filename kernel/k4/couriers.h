
#pragma once

#include "../kernel.h"

namespace Courier
{
// User input courier
const char PROMPT[] = "ABYSS> ";
const char ERROR[] = "ERROR: INVALID COMMAND\r\n";
const char LENGTH_ERROR[] = "ERROR: COMMAND TOO LONG\r\n";
const char QUIT[] = "QUITTING...\r\n";

const int TERMINAL_TIMER_TICKS = 10;

constexpr int CMD_LEN = 64;
void user_input();

// Clock courier.
// This courier is responsible for sending a message to
// the recipient_tid every `repeat` ticks (default 10).
const char CLOCK_COURIER_NAME[] = "CLOCK_COURIER";
void terminal_clock_courier();

const char SENSOR_QUERY_COURIER_NAME[] = "SENSOR_COURIER";
void sensor_query_courier();
}