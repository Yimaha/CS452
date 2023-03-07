#include "mmu.h"
#include "context_switch.h"
using namespace MMU;

static char* const TRANSLATE_TABLE_START = (char*)0x00040000;

static volatile TranslateTable* TABLE_L_0 = (TranslateTable*)(TRANSLATE_TABLE_START);
static volatile TranslateTable* TABLE_L_1 = (TranslateTable*)(TRANSLATE_TABLE_START + sizeof(TranslateTable));
static volatile TranslateTable* TABLE_L_2_0 = (TranslateTable*)(TRANSLATE_TABLE_START + sizeof(TranslateTable) * 2);
static volatile TranslateTable* TABLE_L_2_1 = (TranslateTable*)(TRANSLATE_TABLE_START + sizeof(TranslateTable) * 3);
static volatile TranslateTable* TABLE_L_2_2 = (TranslateTable*)(TRANSLATE_TABLE_START + sizeof(TranslateTable) * 4);
static volatile TranslateTable* TABLE_L_2_3 = (TranslateTable*)(TRANSLATE_TABLE_START + sizeof(TranslateTable) * 5);

void MMU::setup_mmu() {

	uint64_t next_level_table_address = TABLE_ADDR_MASK & (uint64_t)(TABLE_L_1);
	_zero_table(TABLE_L_0);
	TABLE_L_0->row[0] = (next_level_table_address | TABLE); // drop later 3 bytes

	_zero_table(TABLE_L_1);
	next_level_table_address = TABLE_ADDR_MASK & (uint64_t)(TABLE_L_2_0);
	TABLE_L_1->row[0] = (next_level_table_address | TABLE); // drop later 3 bytes

	next_level_table_address = TABLE_ADDR_MASK & (uint64_t)(TABLE_L_2_1);
	TABLE_L_1->row[1] = (next_level_table_address | TABLE); // drop later 3 bytes

	next_level_table_address = TABLE_ADDR_MASK & (uint64_t)(TABLE_L_2_2);
	TABLE_L_1->row[2] = (next_level_table_address | TABLE); // drop later 3 bytes

	next_level_table_address = TABLE_ADDR_MASK & (uint64_t)(TABLE_L_2_3);
	TABLE_L_1->row[3] = (next_level_table_address | TABLE); // drop later 3 bytes

	// now we set up the table, which is pain

	TABLE_L_2_0->row[0] = EXECUTABLE_MEMORY_SETUP;

	uint64_t address_counter = 1; // defined as 47 - 21 bits of the page 2 address, will keep increase by 1 until it fill the entire 4 gb, note that each increment by 1 means increment by 2 mb,
								  // meaning that there will be a total of 2048 increment
	for (int i = 1; i < TABLE_4KB_SIZE_LIMIT; i++) {
		TABLE_L_2_0->row[i] = REGULAR_MEMORY_SETUP | address_counter << 21;
		address_counter += 1;
	}

	for (int i = 0; i < TABLE_4KB_SIZE_LIMIT; i++) {
		TABLE_L_2_1->row[i] = REGULAR_MEMORY_SETUP | address_counter << 21;
		address_counter += 1;
	}
	// technically speaking this table is redundent.
	for (int i = 0; i < TABLE_4KB_SIZE_LIMIT; i++) {
		TABLE_L_2_2->row[i] = DEVICE_MEMORY_SETUP | nGnRnE | address_counter << 21;
		address_counter += 1;
	}

	for (int i = 0; i < TABLE_4KB_SIZE_LIMIT; i++) {
		TABLE_L_2_3->row[i] = DEVICE_MEMORY_SETUP | nGnRnE | address_counter << 21;
		address_counter += 1;
	}

	// we have table but we also have to set up associated register
	mmu_registers((char*)TABLE_L_1);
	// if this line printed we good
	printf("mmu setup complete\r\n");
}

void MMU::_zero_table(volatile TranslateTable* table) {
	for (int i = 0; i < 512; i++) {
		table->row[i] = 0;
	}
}
void MMU::_print_table(volatile TranslateTable* table) {
	for (int i = 0; i < 512; i++) {
		printf("%03d: %016llx ", i, table->row[i]);
		if (i % 4 == 3) {
			printf("\r\n");
		}
	}
}

void MMU::_print_all() {
	printf("translation table 1: \r\n");
	_print_table(TABLE_L_0);
	printf("translation table 2: \r\n");
	_print_table(TABLE_L_1);
	printf("translation table 3: \r\n");
	_print_table(TABLE_L_2_0);
	printf("translation table 4: \r\n");
	_print_table(TABLE_L_2_1);
	printf("translation table 5: \r\n");
	_print_table(TABLE_L_2_2);
	printf("translation table 6: \r\n");
	_print_table(TABLE_L_2_3);
}
