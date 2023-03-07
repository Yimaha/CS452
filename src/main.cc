
#include "interrupt/clock.h"
#include "kernel.h"
#include "mmu.h"
#include "rpi.h"
#include "utils/printf.h"

extern char __bss_start, __bss_end;					   // defined in linker script
extern uintptr_t __init_array_start, __init_array_end; // defined in linker script
typedef void (*funcvoid0_t)();


extern "C" void kmain() {
	printf("init kernel\r\n");
	Kernel kernel = Kernel();
	printf("finished kernel init, started scheduling user tasks\r\n");
	MMU::setup_mmu();
	Interrupt::init_interrupt();
	kernel.start_timer();

	for (;;) {
		// infinite loop, kernel never needs to exit until killed by power switch
		kernel.schedule_next_task(); // tell kernel to schedule next task
		kernel.activate();			 // tell kernel to activate the scheduled task
		kernel.handle();			 // tell kernel to handle the request from task
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
