

#pragma once
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"



namespace UserTask
{
/**
 * Explaination:
 * This class is designed to make test the SRR speed, it runs 4 bytes, 64 bytes, and 256 bytes msg length passing for a total of TASK_TOTAL_CYCLE amount each
 * at the end, check how much micro seconds each operation took.
 *
 * it also tries both direction of either sender first or receiver first, thus you have a total of 6 results
 * when you include this file, you will have compilation warning because certain variables are created in non standard way to force
 * -O3 flag to not skip certain calculation results, thus skipping msg passing
 */

extern "C" void AutoStart();
extern "C" void AutoRedo();
extern "C" void Sender();
extern "C" void Receiver();
extern "C" void Sender1();
extern "C" void Receiver1();
extern "C" void Sender2();
extern "C" void Receiver2();
extern "C" void Sender3();
extern "C" void Receiver3();
extern "C" void Sender4();
extern "C" void Receiver4();
extern "C" void Sender5();
extern "C" void Receiver5();
}