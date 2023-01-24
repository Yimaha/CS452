#include "user_tasks.h"

extern "C" void Task_0() {
    char msg[] = "user task\r\n";
    uart_puts(0, 0, msg, sizeof(msg) - 1);
    yield();
    while (1) {}
}

extern "C" void Task_1() {

}

extern "C" void Task_2() {

}

extern "C" void Task_3() {

}

extern "C" void Task_4() {

}