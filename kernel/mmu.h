#pragma once

#include <stdint.h>
#include "./utils/printf.h"

namespace MMU
{

const uint64_t TABLE_ADDR_MASK = 0xFFFFFFFF << 3;
const uint64_t AF = 1 << 10;
const uint64_t SH = 0b11 << 8;
const uint64_t AP_RW = 1 << 6;
const uint64_t BLOCK = 0b01;
const uint64_t TABLE = 0b11;
const uint64_t nGnRnE = 1 << 2;
const uint64_t ENABLE_CACHE = 2 << 2;
const uint64_t EXECUTABLE_MEMORY_SETUP = AF | SH | ENABLE_CACHE | BLOCK;
const uint64_t REGULAR_MEMORY_SETUP = AF | SH | AP_RW | ENABLE_CACHE | BLOCK;
const uint64_t DEVICE_MEMORY_SETUP = AF | SH | AP_RW | nGnRnE | BLOCK;
const int TABLE_4KB_SIZE_LIMIT = 512;

struct TranslateTable {
	uint64_t row[TABLE_4KB_SIZE_LIMIT];
};


void setup_mmu();
void _zero_table(volatile TranslateTable* table);
void _print_table(volatile TranslateTable* table);
void _print_all();
}


