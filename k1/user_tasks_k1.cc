#include "user_tasks_k1.h"

extern "C" void Task_0() {
    while (1) {
        char msg[] = "entry to user task 0\r\n";
        uart_puts(0, 0, msg, sizeof(msg) - 1);
        Create(2, &Task_1);
        char msg1[] = "Created: task 1\r\n";
        uart_puts(0, 0, msg1, sizeof(msg1) - 1);

        Create(2, &Task_2);
        char msg2[] = "Created: task 2\r\n";
        uart_puts(0, 0, msg2, sizeof(msg2) - 1);

        Create(0, &Task_3);
        char msg3[] = "Created: task 3\r\n";
        uart_puts(0, 0, msg3, sizeof(msg3) - 1);

        Create(0, &Task_4);
        char msg4[] = "Created: task 4\r\n";
        uart_puts(0, 0, msg4, sizeof(msg4) - 1);        
        
        char msg5[] = "FirstUserTask: exiting \r\n";
        uart_puts(0, 0, msg5, sizeof(msg5) - 1); 
        Exit();
    }
}

extern "C" void Task_1() {

    while (1) {
        int id = MyTid();
        int p_id = MyParentTid();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Yield();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Exit();
    }
}

extern "C" void Task_2() {
    while (1) {
        int id = MyTid();
        int p_id = MyParentTid();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Yield();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Exit();
    }
}

extern "C" void Task_3() {
    while (1) {
        int id = MyTid();
        int p_id = MyParentTid();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Yield();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Exit();
    }
}

extern "C" void Task_4() {
    while (1) {
        int id = MyTid();
        int p_id = MyParentTid();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Yield();
        print("my task id: ", 12);
        print_int(id);
        print(" my parent id: ", 15);
        print_int(p_id);
        print("\r\n", 2);

        Exit();
    }
}