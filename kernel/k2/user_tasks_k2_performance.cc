#include "user_tasks_k2_performance.h"
#include "../interrupt/clock.h"
#define TASK_TOTAL_CYCLE 150000

/**
 * Explaination:
 * This class is designed to make test the SRR speed, it runs 4 bytes, 64 bytes, and 256 bytes msg length passing for a total of TASK_TOTAL_CYCLE amount each
 * at the end, check how much micro seconds each operation took.
 *
 * it also tries both direction of either sender first or receiver first, thus you have a total of 6 results
 * when you include this file, you will have compilation warning because certain variables are created in non standard way to force
 * -O3 flag to not skip certain calculation results, thus skipping msg passing
 */

template <const size_t SIZE>
void send_helper(int to) {
	print_int(SIZE);
	char a[] = "test for data sender start: ";
	print(a, sizeof(a) - 1);
	uint64_t start = Clock::system_time();
	char msg[SIZE];
	char reply[SIZE];
	int final_len;
	print_int(start);
	print("\r\n", 2);
	for (int i = 0; i < TASK_TOTAL_CYCLE; i++) {
		final_len = Message::Send::Send(to, msg, SIZE, reply, SIZE);
	}
	uint64_t end = Clock::system_time();
	char a2[] = "test for data sender end: ";
	print(a2, sizeof(a2) - 1);
	print_int(end);
	print("\r\n", 2);
	print("total time consumed: ", 21);
	print_int((end - start) / TASK_TOTAL_CYCLE);
	print("\r\n", 2);
	print("garbage dump: ", 14);
	print_int(final_len);
	print("\r\n", 2);
}

template <const size_t SIZE>
void receive_helper() {
	int from = -1;
	char receiver[SIZE];
	char reply[SIZE];
	int msglen = 0;
	print_int(SIZE);
	char a[] = "test for data receiver start: ";
	print(a, sizeof(a) - 1);
	uint64_t start = Clock::system_time();
	print_int(start);
	print("\r\n", 2);
	for (int i = 0; i < TASK_TOTAL_CYCLE; i++) {
		msglen = Message::Receive::Receive(&from, receiver, SIZE);
		msglen = Message::Reply::Reply(from, reply, SIZE);
	}
	uint64_t end = Clock::system_time();

	char a2[] = "test for data receiver end: ";
	print(a2, sizeof(a2) - 1);
	print_int(end);
	print("\r\n", 2);
	print("total time consumed: ", 21);
	print_int((end - start) / TASK_TOTAL_CYCLE);
	print("\r\n", 2);
	print("garbage dump: ", 14);
	print_int(msglen);
	print("\r\n", 2);
}

extern "C" void UserTask::AutoStart() {
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Sender);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Receiver);
	int final_len = Message::Send::Send(1, nullptr, 0, nullptr, 0);
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Sender1);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Receiver1);
	final_len = Message::Send::Send(1, nullptr, 0, nullptr, 0);
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Sender2);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Receiver2);
	final_len = Message::Send::Send(1, nullptr, 0, nullptr, 0);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Sender3);
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Receiver3);
	final_len = Message::Send::Send(1, nullptr, 0, nullptr, 0);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Sender4);
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Receiver4);
	final_len = Message::Send::Send(1, nullptr, 0, nullptr, 0);
	Task::Create(Priority::HIGH_PRIORITY, &UserTask::Sender5);
	Task::Create(Priority::CRITICAL_PRIORITY, &UserTask::Receiver5);
	print("garbage dump: ", 14);
	print_int(final_len);
	print("\r\n", 2);
	Task::Exit();
}

extern "C" void UserTask::AutoRedo() {
	int from = -1;
	int msglen = Message::Receive::Receive(&from, nullptr, 0);
	msglen = Message::Reply::Reply(from, nullptr, 0);
	msglen = Message::Receive::Receive(&from, nullptr, 0);
	msglen = Message::Reply::Reply(from, nullptr, 0);
	msglen = Message::Receive::Receive(&from, nullptr, 0);
	msglen = Message::Reply::Reply(from, nullptr, 0);
	msglen = Message::Receive::Receive(&from, nullptr, 0);
	msglen = Message::Reply::Reply(from, nullptr, 0);
	msglen = Message::Receive::Receive(&from, nullptr, 0);
	msglen = Message::Reply::Reply(from, nullptr, 0);
	print("garbage dump: ", 14);
	print_int(msglen);
	print("\r\n", 2);
	Task::Exit();
}

extern "C" void UserTask::Sender() {
	send_helper<4>(3);
	Task::Exit();
}

extern "C" void UserTask::Receiver() {
	receive_helper<4>();
	Task::Exit();
}

extern "C" void UserTask::Sender1() {
	send_helper<64>(5);
	Task::Exit();
}

extern "C" void UserTask::Receiver1() {
	receive_helper<64>();
	Task::Exit();
}

extern "C" void UserTask::Sender2() {
	send_helper<256>(7);
	Task::Exit();
}

extern "C" void UserTask::Receiver2() {
	receive_helper<256>();
	Task::Exit();
}

extern "C" void UserTask::Sender3() {
	send_helper<4>(9);
	Task::Exit();
}

extern "C" void UserTask::Receiver3() {
	receive_helper<4>();
	Task::Exit();
}

extern "C" void UserTask::Sender4() {
	send_helper<64>(11);
	Task::Exit();
}

extern "C" void UserTask::Receiver4() {
	receive_helper<64>();
	Task::Exit();
}

extern "C" void UserTask::Sender5() {
	send_helper<256>(13);
	Task::Exit();
}

extern "C" void UserTask::Receiver5() {
	receive_helper<256>();
	Task::Exit();
}
