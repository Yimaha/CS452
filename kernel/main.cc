
#include "kernel.h"
#include "rpi.h"
#include "utils/printf.h"

#define NL uart_puts(0, 0, "\r\n", 2)

extern char __bss_start, __bss_end;					   // defined in linker script
extern uintptr_t __init_array_start, __init_array_end; // defined in linker script
typedef void (*funcvoid0_t)();


// struct GICD {
// 	uint32_t GICD_CTLR;

// 	// Fill up space to 0x100
// 	uint32_t _unused[63];
// 	uint32_t GICD_ISENABLERN[16];

// 	// Fill up space to 0x800
// 	uint32_t _unused2[432];
// 	uint32_t GICD_ITARGETSRN[64];
// };

// static char* const GIC_BASE = (char*)0xFF840000;
// static volatile GICD* const gicd = (GICD*)(GIC_BASE + 0x1000);


// void calculate_gicd_isenable(uint32_t m) {
// 	uint32_t n = m / 32;
// 	uint32_t* address = (uint32_t*)(0xFF841100 + 4*n);
// 	// print_int((uint64_t)address);
// 	*address = 1 << (m % 32);
// }

// void calculate_GICD_ITARGETSRn(uint32_t m) {
// 	uint32_t n = m/4;
// 	uint32_t* address = (uint32_t*)(0xFF841800 + 4*n);
// 	// print_int((uint64_t)address);
// 	*address = 1 << (8 * (m % 4));

// }

// void enable_interrupt() {
// 	// there are 3 phases of enable interrupt
// 	// enable the clock to be interrupt (physcial)
// 	// we need to make sure at gic level, the communication channel is open, targeting timer 1
// 	// first, enable GICD_ISENABLERn on the clock, the calculated result is 0x10c as offset
// 	char* const GIC_BASE_2 = (char*)0xFF840000;
// 	*(uint32_t*)(GIC_BASE_2 + 0x1000) = 1;
// 	*(uint32_t*)(GIC_BASE_2 + 0x2000) = 1;
// 	// gicd->GICD_ISENABLERN[97 / 32] = 1 << (97 % 32);
// 	// gicd->GICD_ITARGETSRN[97 / 4] = 1 << (8*(97 % 4));

// 	print("wtf is going on\r\n", 17);
// 	calculate_gicd_isenable(97);
// 	// // also setup GICD_ITARGETSRn to route to cpu 0 
// 	calculate_GICD_ITARGETSRn(97);
// 	set_comparator(1, clo() + 1000000);
// 	print("interrupt_enable\r\n", 18);
// }


extern "C" void kmain() {
	// char m1[] = "init kernel\r\n";
	// uart_puts(0, 0, m1, sizeof(m1) - 1);
	Kernel kernel = Kernel();
	char m2[] = "finished kernel init, started scheduling user tasks\r\n";
	uart_puts(0, 0, m2, sizeof(m2) - 1);
	enable_interrupts();
	set_comparator(clo() + 1000000);
	for (;;) // infinite loop, kernel never needs to exit until killed by power switch
	{
		kernel.schedule_next_task(); // tell kernel to schedule next task
		kernel.activate();			 // tell kernel to activate the scheduled task
		kernel.handle();			 // tell kernel to handle the request from task
#ifdef DEBUG
		char m3[] = "completed kernel cycle\r\n"; // logging that is useful, but should be removed later
		print(m3, sizeof(m3) - 1);
#endif
	}
}

int main() {
	init_gpio();
	init_spi(0);
	init_uart(0);

	for (funcvoid0_t* ctr = (funcvoid0_t*)&__init_array_start; ctr < (funcvoid0_t*)&__init_array_end; ctr += 1)
		(*ctr)();

	kmain(); // where the actual magic happens
	return 0;
}
