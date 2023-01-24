#include "user_tasks.h"

extern "C" void Task_0() {

    while (1) {
        char msg[] = "user task 1\r\n";
        uart_puts(0, 0, msg, sizeof(msg) - 1);
        for (int i = 0; i < 3000000; ++i) asm volatile("yield");
        yield();
    }
}

extern "C" void Task_1() {

    while (1) {
        char msg[] = "user task 2\r\n";
        uart_puts(0, 0, msg, sizeof(msg) - 1);
        for (int i = 0; i < 3000000; ++i) asm volatile("yield");
        yield();
    }
}

extern "C" void Task_2() {

}

extern "C" void Task_3() {

}

extern "C" void Task_4() {

}