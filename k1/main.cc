
#include "buffer.h"
#include "context_switch.h"
#include "kernel.h"
#include "rpi.h"

#define NL uart_puts(0, 0, "\r\n", 2)

extern char __bss_start, __bss_end;					   // defined in linker script
extern uintptr_t __init_array_start, __init_array_end; // defined in linker script
typedef void (*funcvoid0_t)();

extern "C" void kmain() {
	char m1[] = "init kernel\r\n";
	uart_puts(0, 0, m1, sizeof(m1) - 1);
	Kernel kernel = Kernel();
	char m2[] = "finished kernel init, started scheduling user tasks\r\n";

	uart_puts(0, 0, m2, sizeof(m2) - 1);
	for (;;) // infinite loop, kernel never needs to exit until killed by power switch
	{
		kernel.schedule_next_task();			  // tell kernel to schedule next task
		kernel.activate();						  // tell kernel to activate the scheduled task
		kernel.handle();						  // tell kernel to handle the request from task
		char m3[] = "completed kernel cycle\r\n"; // logging that is useful, but should be removed later
		uart_puts(0, 0, m3, sizeof(m3) - 1);
	}
}

int main() {
	init_gpio();
	init_spi(0);
	init_uart(0);

	for (funcvoid0_t* ctr = (funcvoid0_t*)&__init_array_start; ctr < (funcvoid0_t*)&__init_array_end; ctr += 1)
		(*ctr)();

#ifdef DEBUG
	print_int(69696969);
	NL;
#endif

	kmain(); // where the actual magic happens
	return 0;
}
