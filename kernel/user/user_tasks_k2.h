

#pragma once
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"

namespace UserTask
{
// this a set of dummy user processes I made
// feel free to use this as template for creating task in future assignment
extern "C" void low_priority_task();
extern "C" void Sender1();
extern "C" void Sender2();
extern "C" void Receiver();
}