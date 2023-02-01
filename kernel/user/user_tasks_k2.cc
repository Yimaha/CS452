#include "user_tasks_k2.h"

extern "C" void UserTask::low_priority_task() {
	while (1) {
		char msg[] = "low prioirity task that just keep spinning\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		Task::Yield();
	}
}

extern "C" void UserTask::Sender1() {
	while (1) {
		char m1[] = "in sender 1, ready to send\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		char msg[] = "This is a message from sender 1\r\n";
		char reply[30];
		int final_len = MessagePassing::Send::Send(3, msg, sizeof(msg), reply, 30);
		uart_puts(0, 0, reply, final_len - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}

extern "C" void UserTask::Sender2() {
	while (1) {
		char m1[] = "in sender 2, ready to send\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		char msg[] = "This is a message from sender 2\r\n";
		char reply[30];
		int final_len = MessagePassing::Send::Send(3, msg, sizeof(msg), reply, 30);
		uart_puts(0, 0, reply, final_len - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}

extern "C" void UserTask::Receiver() {
	while (1) {
		char m1[] = "in receiver, ready to receive\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		int from = -1;
		char receiver[100];
		int msglen = MessagePassing::Receive::Receive(&from, receiver, 100);
		print(receiver, msglen - 1);
		char m2[] = "recieved, ready to reply\r\n";
		uart_puts(0, 0, m2, sizeof(m2) - 1);
		msglen = MessagePassing::Reply::Reply(from, "eyy yo what's up homie\r\n", 25);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}