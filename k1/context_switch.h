
#pragma once
#include <stdint.h>

extern "C" void check_sp(); // used to set the current sp, which is kinda useful at the beginning
extern "C" uint16_t first_el0_entry(char * userSP, void (*pc) ());
 
