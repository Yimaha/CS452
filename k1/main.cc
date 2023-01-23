
#include "rpi.h"
#include "buffer.h"
#include "context_switch.h"

#define NL uart_puts(0, 0, "\r\n", 2)

extern char __bss_start, __bss_end; // defined in linker script
const char hello[] = "Hello world";

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

int main() {
    init_gpio();
    init_spi(0);
    init_uart(0);

    char msg[] = "stack text\r\n";

    char kernel_stack [6000] = {0}; // placeholder for kernel stack
    char user_stack [5000] = {0}; // placeholder for user stack
    for (int i = 0; i < 5000; i++) {
        kernel_stack[i] = 0x3e;
        user_stack[i] = 0x3e;
    }
    
    for (size_t j = 0; j < sizeof(hello); j++) {
        kernel_stack[j] = hello[j];
    }

    uart_puts(0, 0, msg, sizeof(msg) - 1);

    // try the el0_entry
    val_print((uint64_t)user_stack);
    el0_entry(user_stack);

    uart_puts(0, 0, "el0_entry returned\r\n", 20);
    uart_puts(0, 0, msg, sizeof(msg) - 1); // This will be garbage

    return 0;
}
