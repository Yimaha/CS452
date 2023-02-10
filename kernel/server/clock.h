
#pragma once
#include "../kernel.h"
#include "../rpi.h"

namespace Clock
{

const int MICROSECONDS_PER_TICK = 10000;

void clock_server();
}