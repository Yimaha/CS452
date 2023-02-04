#pragma once
#define USER_STACK_SIZE 131072 // a total of 128kb

#include "context_switch.h"
#include "rpi.h"
#include "scheduler.h"
#include "utils/buffer.h"
#include "utils/utility.h"
#include <cstdint>

// used for storing receive infos
struct MessageReceiver {
	int* from;
	char* loc;
	int len;
};

// used for queueing up message
struct Message {
	int from;
	char* loc;
	int len;
};

class TaskDescriptor {
public:
	enum TaskState { ERROR = 0, ACTIVE = 1, READY = 2, ZOMBIE = 3, SEND_BLOCK = 4, RECEIVE_BLOCK = 5, REPLY_BLOCK = 6, EVENT_BLOCK = 7 };
	TaskDescriptor(int id, int parent_id, int priority, void (*pc)());
	// message related api
	void queue_message(int from, char* msg, int message_length); // queue_up a message
	bool have_message();
	int fill_message(Message msg, int* from, char* msg_container, int msglen);
	int fill_response(int from, char* msg, int msglen); // the reverse of last function, fill the response buffer
	Message pop_inbox();
	// state modifying api
	InterruptFrame* to_active();
	void to_ready(int system_response, Scheduler* scheduler);
	bool kill();
	void to_send_block(char* reply, int replylen);
	void to_receive_block(int* from, char* msg, int msglen);
	void to_reply_block();
	void to_reply_block(char* reply, int replylen);
	// k3 will have to_event_block

	// state checking api
	bool is_active();
	bool is_ready();
	bool is_zombie();
	bool is_send_block();
	bool is_receive_block();
	bool is_reply_block();

	const int task_id;
	const int parent_id; // id = -1 means no parent

	friend class Kernel; // allow kernel to access protected functions (and potential future unit tests)
protected:
	// debug api
	void show_info();

private:
	TaskState state;
	int priority;
	int system_call_result;
	bool initialized;
	void (*pc)();						 // program counter, but typically only used as a reference value to see where the start of the program is
	MessageReceiver response;			 // used to store response if task decided to call send, or receive (anything that can block)
	RingBuffer<Message> inbox;			 // receiver of message
	char* sp;							 // stack pointer
	char* kernel_stack[USER_STACK_SIZE]; // approximately 16 kbytes per stack
};
