
#include "rpi.h"
#include "buffer.h"
#include "context_switch.h"

extern char __bss_start, __bss_end; // defined in linker script

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

    char kernel_stack [6000] = {0}; // placeholder for kernel stack
    char user_stack [5000] = {0}; // placeholder for user stack
    kernel_stack[0] = 'H';
    kernel_stack[1] = 'E';
    for (int i = 0; i < 5000; i++) {
        kernel_stack[i] = 0x3e;
    }

    setSP(kernel_stack);
    bar();


    // char msg1[] = "Hello world, this is iotest (" __TIME__ ")\r\nPress 'q' to reboot\r\n";
    // uart_puts(0, 0, msg1, sizeof(msg1) - 1);
    // int r = bar(1, 2);
    // if (r == 3) {
    //     char prompt[] = "PI> ";
    //     uart_puts(0, 0, prompt, sizeof(prompt) - 1);
    // }
    // char c = ' ';
    // while (c != 'q') {
    //     c = uart_getc(0, 0);
    //     if (c == '\r') {
    //         uart_puts(0, 0, "\r\n", 2);
    //         // uart_puts(0, 0, prompt, sizeof(prompt) - 1);
    //     } else {
    //         uart_putc(0, 0, c);
    //     }
    // }
    // uart_puts(0, 0, "\r\n", 2);

    return 0;
}
