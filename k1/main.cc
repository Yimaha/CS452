
#include "rpi.h"
#include "buffer.h"
#include "context_switch.h"
#include "kernel.h"

#define NL uart_puts(0, 0, "\r\n", 2)

extern char __bss_start, __bss_end; // defined in linker script

extern uintptr_t __init_array_start, __init_array_end; // defined in linker script;
typedef void (*funcvoid0_t)();

/*
void kmain() {
    initialize();  // includes starting the first user task
    for (;;) {
        currtask = schedule();
        request = activate(currtask);
        handle(request);
    }
}
*/


extern "C" void kmain() {
    char m1[] = "init kernel \r\n";
    uart_puts(0, 0, m1, sizeof(m1) - 1);
    Kernel kernel = Kernel();
    char m2[] = "finish kernel init\r\n";
    uart_puts(0, 0, m2, sizeof(m2) - 1);
    for (int i = 0; i < 100; i++) {
        kernel.schedule_next_task(); // tell kernel to schedule next task
        kernel.activate(); // tell kernel to activate the scheduled task
        kernel.handle(); // tell kernel to handle the request from task
        char m3[] = "complete kernel cycle \r\n";
        uart_puts(0, 0, m3, sizeof(m3) - 1);
    }
}

int main() {
    init_gpio();
    init_spi(0);
    init_uart(0);


    for (funcvoid0_t* ctr = (funcvoid0_t *)&__init_array_start; ctr < (funcvoid0_t *)&__init_array_end; ctr += 1) (*ctr)();

    kmain(); // where the actual magic happens
    return 0;
}





