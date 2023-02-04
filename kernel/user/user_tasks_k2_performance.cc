#include "user_tasks_k2_performance.h"

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


template<const size_t SIZE>
void send_helper(int to) {
	print_int(SIZE);
	char a[] = "test for data sender start: " ;
	print(a, sizeof(a)-1);
	uint64_t start = time();
	print_int(start);
	print("\r\n", 2);
	char msg[SIZE];
	char reply[SIZE];
    int final_len;
	for (int i = 0; i < TASK_TOTAL_CYCLE; i++) {
		final_len = MessagePassing::Send::Send(to, msg, SIZE, reply, SIZE);
	}
	char a2[] = "test for data sender end: " ;
	print(a2, sizeof(a2)-1);
	uint64_t end = time();
	print_int(end);
	print("\r\n", 2);
	print("total time consumed: ", 21);
	print_int((end - start)/TASK_TOTAL_CYCLE);
	print("\r\n", 2);
    print("garbage dump: ", 14);
    print_int(final_len);
    print("\r\n", 2);
}

template<const size_t SIZE>
void receive_helper() {
	print_int(SIZE);
	char a[] = "test for data receiver start: " ;
	print(a, sizeof(a)-1);
	uint64_t start = time();
	print_int(start);
	print("\r\n", 2);
	int from = -1;
	char receiver[SIZE];
	char reply[SIZE];
	int msglen = 0;
	for (int i = 0; i < TASK_TOTAL_CYCLE; i++) {
		msglen = MessagePassing::Receive::Receive(&from, receiver, SIZE);
		msglen = MessagePassing::Reply::Reply(from, reply, SIZE);
	}
	
	char a2[] = "test for data receiver end: " ;
	print(a2, sizeof(a2)-1);
	uint64_t end = time();
	print_int(end);
	print("\r\n", 2);
	print("total time consumed: ", 21);
	print_int((end - start)/TASK_TOTAL_CYCLE);
	print("\r\n", 2);
    print("garbage dump: ", 14);
    print_int(msglen);
    print("\r\n", 2);
}


extern "C" void UserTask::AutoStart() {
	Task::Creation::Create(1, &UserTask::Sender);
	Task::Creation::Create(2, &UserTask::Receiver);
	int final_len = MessagePassing::Send::Send(1, nullptr, 0,  nullptr, 0);
	Task::Creation::Create(1, &UserTask::Sender1);
	Task::Creation::Create(2, &UserTask::Receiver1);
	final_len = MessagePassing::Send::Send(1, nullptr, 0,  nullptr, 0);
	Task::Creation::Create(1, &UserTask::Sender2);
	Task::Creation::Create(2, &UserTask::Receiver2);
	final_len = MessagePassing::Send::Send(1, nullptr, 0,  nullptr, 0);
	Task::Creation::Create(2, &UserTask::Sender3);
	Task::Creation::Create(1, &UserTask::Receiver3);
	final_len = MessagePassing::Send::Send(1, nullptr, 0,  nullptr, 0);
	Task::Creation::Create(2, &UserTask::Sender4);
	Task::Creation::Create(1, &UserTask::Receiver4);
	final_len = MessagePassing::Send::Send(1, nullptr, 0,  nullptr, 0);
	Task::Creation::Create(2, &UserTask::Sender5);
	Task::Creation::Create(1, &UserTask::Receiver5);
    print("garbage dump: ", 14);
    print_int(final_len);
    print("\r\n", 2);
	Task::Destruction::Exit();
}

extern "C" void UserTask::AutoRedo() {
	int from = -1;
	int msglen = MessagePassing::Receive::Receive(&from, nullptr, 0);
	msglen = MessagePassing::Reply::Reply(from, nullptr, 0);
	msglen = MessagePassing::Receive::Receive(&from, nullptr, 0);
	msglen = MessagePassing::Reply::Reply(from, nullptr, 0);
	msglen = MessagePassing::Receive::Receive(&from, nullptr, 0);
	msglen = MessagePassing::Reply::Reply(from, nullptr, 0);
	msglen = MessagePassing::Receive::Receive(&from, nullptr, 0);
	msglen = MessagePassing::Reply::Reply(from, nullptr, 0);
	msglen = MessagePassing::Receive::Receive(&from, nullptr, 0);
	msglen = MessagePassing::Reply::Reply(from, nullptr, 0);
    print("garbage dump: ", 14);
    print_int(msglen);
    print("\r\n", 2);
	Task::Destruction::Exit();
}


extern "C" void UserTask::Sender() {
	send_helper<4>(3);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver() {
	receive_helper<4>();
	Task::Destruction::Exit();
}

extern "C" void UserTask::Sender1() {
	send_helper<64>(5);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver1() {
	receive_helper<64>();
	Task::Destruction::Exit();
}

extern "C" void UserTask::Sender2() {
	send_helper<256>(7);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver2() {
	receive_helper<256>();
	Task::Destruction::Exit();
}


extern "C" void UserTask::Sender3() {
	send_helper<4>(9);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver3() {
	receive_helper<4>();
	Task::Destruction::Exit();
}

extern "C" void UserTask::Sender4() {
	send_helper<64>(11);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver4() {
	receive_helper<64>();
	Task::Destruction::Exit();
}

extern "C" void UserTask::Sender5() {
	send_helper<256>(13);
	Task::Destruction::Exit();
}

extern "C" void UserTask::Receiver5() {
	receive_helper<256>();
	Task::Destruction::Exit();
}



