#include "user_tasks_k2.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
#include "rps_server.h"

extern "C" void UserTask::first_user_task() {
	while (true) {
		// Create the name server
		char msg[] = "creating name server\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		Task::Creation::Create(1, &Name::name_server);

		// Task::Creation::Create(3, &UserTask::Sender1N);
		// Task::Creation::Create(3, &UserTask::Sender2N);
		// Task::Creation::Create(2, &UserTask::ReceiverN);

		// Create the RPS server
		char msg2[] = "creating rps server\r\n";
		uart_puts(0, 0, msg2, sizeof(msg2) - 1);
		Task::Creation::Create(2, &RockPaperScissors::RPSServer);

		// Create the RPS clients
		for (int i = 0; i < 5; ++i) {
			char msg3[] = "creating rps client\r\n";
			uart_puts(0, 0, msg3, sizeof(msg3) - 1);
			Task::Creation::Create(3, &RockPaperScissors::RPSClient);
		}

		char msg5[] = "exiting task 0\r\n";
		uart_puts(0, 0, msg5, sizeof(msg5) - 1);
		Task::Destruction::Exit();
	}
}

extern "C" void UserTask::low_priority_task() {
	while (true) {
		char msg[] = "low priority task that just keeps spinning\r\n";
		uart_puts(0, 0, msg, sizeof(msg) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		Task::Yield();
	}
}

extern "C" void UserTask::Sender1N() {
	const char name[] = "Sender1N";
	int tid = Name::RegisterAs(name);

	while (1) {
		char m1[] = "in sender 1, ready to send\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		char m2[] = "my tid is: ";
		uart_puts(0, 0, m2, sizeof(m2) - 1);
		print_int(tid);
		print("\r\n", 2);

		char msg[] = "This is a message from sender 1\r\n";
		char reply[30];
		int recv_tid = Name::WhoIs("ReceiverN");
		char m3[] = "receiver tid is: ";
		uart_puts(0, 0, m3, sizeof(m3) - 1);
		print_int(recv_tid);
		print("\r\n", 2);
		while (recv_tid == Name::Exception::NAME_NOT_REGISTERED) {
			Task::Yield();
			recv_tid = Name::WhoIs("ReceiverN");
		}

		int final_len = MessagePassing::Send::Send(recv_tid, msg, sizeof(msg), reply, 30);
		uart_puts(0, 0, reply, final_len - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}

extern "C" void UserTask::Sender2N() {
	const char name[] = "Sender2N";
	int tid = Name::RegisterAs(name);

	while (1) {
		char m1[] = "in sender 2, ready to send\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		char m2[] = "my tid is: ";
		uart_puts(0, 0, m2, sizeof(m2) - 1);
		print_int(tid);
		print("\r\n", 2);

		char msg[] = "This is a message from sender 2\r\n";
		char reply[30];
		int recv_tid = Name::WhoIs("ReceiverN");
		char m3[] = "receiver tid is: ";
		uart_puts(0, 0, m3, sizeof(m3) - 1);
		print_int(recv_tid);
		print("\r\n", 2);
		while (recv_tid == Name::Exception::NAME_NOT_REGISTERED) {
			Task::Yield();
			recv_tid = Name::WhoIs("ReceiverN");
		}

		int final_len = MessagePassing::Send::Send(recv_tid, msg, sizeof(msg), reply, 30);
		uart_puts(0, 0, reply, final_len - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}

extern "C" void UserTask::ReceiverN() {
	const char name[] = "ReceiverN";
	int tid = Name::RegisterAs(name);

	while (1) {
		char m0[] = "in receiver, ready to receive\r\n";
		uart_puts(0, 0, m0, sizeof(m0) - 1);
		char m1[] = "my tid is: ";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
		print_int(tid);
		print("\r\n", 2);

		int from = -1;
		char receiver[100];
		int msglen = MessagePassing::Receive::Receive(&from, receiver, 100);
		print(receiver, msglen - 1);
		char m2[] = "received, ready to reply\r\n";
		uart_puts(0, 0, m2, sizeof(m2) - 1);
		msglen = MessagePassing::Reply::Reply(from, "eyy yo what's up homie\r\n", 25);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
	}
}
