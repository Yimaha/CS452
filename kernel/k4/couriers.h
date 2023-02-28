
#pragma once

#include "../kernel.h"

namespace Courier
{
const char PROMPT[] = "ABYSS> ";
const char ERROR[] = "ERROR: INVALID COMMAND\r\n";
const char LENGTH_ERROR[] = "ERROR: COMMAND TOO LONG\r\n";
const char QUIT[] = "QUITTING...\r\n";

constexpr int CMD_LEN = 64;
void user_input();
}