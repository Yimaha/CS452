#include "user_tasks_k1.h"


// The sample task requested by k1
// TODO: since you receive an int on create,
// we should check if create returns something,
// maybe a sentinel value like -1
extern "C" void Task_0()
{
    while (1)
    {
        char msg[] = "entered into user task 0\r\n";
        uart_puts(0, 0, msg, sizeof(msg) - 1);
        Create(2, &Sub_Task);
        char msg1[] = "created: task 1\r\n";
        uart_puts(0, 0, msg1, sizeof(msg1) - 1);

        Create(2, &Sub_Task);
        char msg2[] = "created: task 2\r\n";
        uart_puts(0, 0, msg2, sizeof(msg2) - 1);

        Create(0, &Sub_Task);
        char msg3[] = "created: task 3\r\n";
        uart_puts(0, 0, msg3, sizeof(msg3) - 1);

        Create(0, &Sub_Task);
        char msg4[] = "created: task 4\r\n";
        uart_puts(0, 0, msg4, sizeof(msg4) - 1);

        char msg5[] = "exiting task 0\r\n";
        uart_puts(0, 0, msg5, sizeof(msg5) - 1);
        Exit();
    }
}

extern "C" void Sub_Task()
{
    int id = MyTid();
    int p_id = MyParentTid();
    print("my task id: ", 12);
    print_int(id);
    print("; my parent id: ", 16);
    print_int(p_id);
    print("\r\n", 2);

    Yield();
    print("my task id: ", 12);
    print_int(id);
    print("; my parent id: ", 16);
    print_int(p_id);
    print("\r\n", 2);

    Exit();
}
