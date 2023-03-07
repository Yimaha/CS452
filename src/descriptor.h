#pragma once
#include "context_switch.h"
#include "rpi.h"
#include "scheduler.h"
#include "utils/buffer.h"
#include "utils/printf.h"
#include "utils/utility.h"
#include <cstdint>

namespace Descriptor
{

const uint64_t USER_STACK_SIZE = 131072; // a total of 128kb
const uint64_t INBOX_SIZE = 64;

struct MessageReceiver {
	int* from;
	char* loc;
	int len;
};

// used for queueing up message
struct MessageStruct {
	int from;
	char* loc;
	int len;
};

class TaskDescriptor {
public:
	enum TaskState { ERROR = 0, ACTIVE = 1, READY = 2, ZOMBIE = 3, SEND_BLOCK = 4, RECEIVE_BLOCK = 5, REPLY_BLOCK = 6, EVENT_BLOCK = 7, INTERRUPTED = 8, NOT_INITIALIZED = 9 };
	TaskDescriptor(int id, int parent_id, Priority priority, void (*pc)());
	// message related api
	void queue_message(int from, char* msg, int message_length); // queue_up a message
	bool have_message();
	int fill_message(MessageStruct msg, int* from, char* msg_container, int msglen);
	int fill_response(int from, char* msg, int msglen); // the reverse of last function, fill the response buffer
	MessageStruct pop_inbox();
	char* get_event_buffer();
	// state modifying api
	InterruptFrame* to_active();
	void to_ready(int system_response, Task::Scheduler* scheduler);
	void to_interrupted(Task::Scheduler* scheduler);
	bool kill();
	void to_send_block(char* reply, int replylen);
	void to_receive_block(int* from, char* msg, int msglen);
	void to_reply_block();
	void to_reply_block(char* reply, int replylen);
	// k3 will have to_event_block
	void to_event_block();
	void to_event_block_with_buffer(char* buffer);

	// state checking api
	bool is_active();
	bool is_ready();
	bool is_zombie();
	bool is_send_block();
	bool is_receive_block();
	bool is_reply_block();
	bool is_event_block();
	bool is_interrupted();
	bool is_not_initialized();

	const int task_id;
	const int parent_id; // id = -1 means no parent
	Priority priority;

protected:
	// debug api
	void show_info();

private:
	TaskState state;
	int system_call_result;

	void (*pc)();								 // program counter, but typically only used as a reference value to see where the start of the program is
	MessageReceiver response;					 // used to store response if task decided to call send, or receive
	char* event_buffer;							 // used to store response if block on event that need reading (note that I could use response, but for good practice, no)
	etl::queue<MessageStruct, INBOX_SIZE> inbox; // receiver of message
	char* sp;									 // stack pointer
	char* spsr;									 // saved program status register
	char* kernel_stack[USER_STACK_SIZE];		 // approximately 128 kbytes per stack
};

inline InterruptFrame* TaskDescriptor::to_active() {

	if (is_not_initialized()) {
		// startup task, has no parameter or handling
		state = TaskDescriptor::TaskState::ACTIVE;
		sp = (char*)first_el0_entry(sp, pc);
	} else if (is_interrupted()) {
		// interrupted task, has to restore the context
		state = TaskDescriptor::TaskState::ACTIVE;
		sp = (char*)to_user_interrupted(sp, spsr, pc);
	} else {
		state = TaskDescriptor::TaskState::ACTIVE;
		sp = (char*)to_user(system_call_result, sp, spsr);
	}

	InterruptFrame* intfr = reinterpret_cast<InterruptFrame*>(sp);
	spsr = reinterpret_cast<char*>(intfr->spsr);
	pc = reinterpret_cast<void (*)()>(intfr->pc);
	return intfr;
}
}
// used for storing receive infos
