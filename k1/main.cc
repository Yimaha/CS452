
#include "rpi.h"
#include "buffer.h"
#include "context_switch.h"
#include "kernel.h"

#define NL uart_puts(0, 0, "\r\n", 2)

extern char __bss_start, __bss_end; // defined in linker script
const char hello[] = "Hello world";

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

extern "C" void user_task() {
    char msg[] = "user task\r\n";
    uart_puts(0, 0, msg, sizeof(msg) - 1);
    while (1) {}
}

extern "C" void kmain() {
    char m1[] = "init kernel \r\n";
    uart_puts(0, 0, m1, sizeof(m1) - 1);
    Kernel kernel = Kernel();
    char m2[] = "finish kernel \r\n";
    uart_puts(0, 0, m2, sizeof(m2) - 1);
    for (;;) {
        // kernel.schedule_next_task();
        uint16_t request = kernel.activate();
    }
}

int main() {
    init_gpio();
    init_spi(0);
    init_uart(0);


    for (funcvoid0_t* ctr = (funcvoid0_t *)&__init_array_start; ctr < (funcvoid0_t *)&__init_array_end; ctr += 1) (*ctr)();
    char msg[] = "test print\r\n";
    uart_puts(0, 0, msg, sizeof(msg) - 1);
    kmain(); // where the actual magic happens
    return 0;
}
