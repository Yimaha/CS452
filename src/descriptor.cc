
#include "descriptor.h"
#include "kernel.h"
#include "utils/printf.h"

using namespace Descriptor;

/**
 * initialization,
 */
TaskDescriptor::TaskDescriptor(int id, int parent_id, Priority priority, void (*pc)())
	: task_id { id }
	, parent_id { parent_id }
	, priority { priority }
	, state { TaskState::NOT_INITIALIZED }
	, system_call_result { 0x0 } // used to reply from kernel function
	, pc { pc } {
	sp = (char*)&kernel_stack[USER_STACK_SIZE]; // aligned to 8 bytes, exactly 4kb is used for user stack
}

void TaskDescriptor::queue_message(int from, char* msg, int msglen) {
	if (inbox.full()) {
		Task::_KernelCrash("inbox is full for task id %d", task_id);
	}
	inbox.push(MessageStruct { from, msg, msglen });
}

bool TaskDescriptor::have_message() {
	return !inbox.empty();
}

int TaskDescriptor::fill_message(MessageStruct msg, int* from, char* msg_container, int msglen) {
	memcpy(msg_container, msg.loc, min(msglen, msg.len));
	*from = msg.from;
	return msg.len;
}

int TaskDescriptor::fill_response(int from, char* msg, int msglen) {
	int min_len = min(response.len, msglen);
	memcpy(response.loc, msg, min_len);
	*response.from = from;
	return min_len;
}

MessageStruct TaskDescriptor::pop_inbox() {
	MessageStruct msg = inbox.front();
	inbox.pop();
	return msg;
}

char* TaskDescriptor::get_event_buffer() {
	return event_buffer;
}

void TaskDescriptor::to_ready(int system_response, Task::Scheduler* scheduler) {
#ifdef OUR_DEBUG
	if (state == ACTIVE || state == SEND_BLOCK || state == RECEIVE_BLOCK || state == REPLY_BLOCK || state == EVENT_BLOCK || state == INTERRUPTED) // ignoring event block for k2
	{
#endif
		state = READY;
		system_call_result = system_response;
		scheduler->add_task(priority, task_id); // queue back into scheduler

#ifdef OUR_DEBUG
	} else {
		print("unblock is called on task that is not blocked!\r\n", 48);
		Task::_KernelCrash("task id: %d, state: %d\r\n", task_id, state);
	}
#endif
}

bool TaskDescriptor::kill() {
	if (!is_zombie()) {
		state = TaskState::ZOMBIE;
		return true;
	}
	return false;
}

void TaskDescriptor::to_send_block(char* reply, int replylen) {
	state = TaskDescriptor::TaskState::SEND_BLOCK;
	response = { nullptr, reply, replylen };
}

void TaskDescriptor::to_receive_block(int* from, char* msg, int msglen) {
	state = TaskDescriptor::TaskState::RECEIVE_BLOCK;
	response = { from, msg, msglen };
}

void TaskDescriptor::to_reply_block() {
	state = TaskDescriptor::TaskState::REPLY_BLOCK;
}

void TaskDescriptor::to_reply_block(char* reply, int replylen) {
	state = TaskDescriptor::TaskState::REPLY_BLOCK;
	response = { nullptr, reply, replylen };
}

void TaskDescriptor::to_event_block() {
	state = TaskDescriptor::TaskState::EVENT_BLOCK;
}

void TaskDescriptor::to_event_block_with_buffer(char* buffer) {
	event_buffer = buffer;
	state = TaskDescriptor::TaskState::EVENT_BLOCK;
}
void TaskDescriptor::to_interrupted(Task::Scheduler* scheduler) {
	state = TaskDescriptor::TaskState::INTERRUPTED;
	scheduler->add_task(priority, task_id);
}

/**
 * good way to check if a process is still alive
 */

bool TaskDescriptor::is_active() {
	return state == TaskState::ACTIVE;
}

bool TaskDescriptor::is_ready() {
	return state == TaskState::READY;
}

bool TaskDescriptor::is_zombie() {
	return state == TaskState::ZOMBIE;
}

bool TaskDescriptor::is_send_block() {
	return state == TaskState::SEND_BLOCK;
}

bool TaskDescriptor::is_receive_block() {
	return state == TaskState::RECEIVE_BLOCK;
}

bool TaskDescriptor::is_reply_block() {
	return state == TaskState::REPLY_BLOCK;
}

bool TaskDescriptor::is_event_block() {
	return state == TaskState::EVENT_BLOCK;
}

bool TaskDescriptor::is_interrupted() {
	return state == TaskState::INTERRUPTED;
}

bool TaskDescriptor::is_not_initialized() {
	return state == TaskState::NOT_INITIALIZED;
}

/**
 * Debug function used to print out helpful state of a Task descriptor
 * note that this need an update to fill all missing fields
 */
void Descriptor::TaskDescriptor::show_info() {
	char m1[] = "printing status of TaskDescriptor\r\n";
	uart_puts(0, 0, m1, sizeof(m1) - 1);
	char m2[] = "ID: ";
	uart_puts(0, 0, m2, sizeof(m2) - 1);
	print_int(task_id);
	print("\r\n", 2);
	char m3[] = "Parent ID: ";
	uart_puts(0, 0, m3, sizeof(m3) - 1);
	print_int(parent_id);
	print("\r\n", 2);
	char m4[] = "priority: ";
	uart_puts(0, 0, m4, sizeof(m4) - 1);
	print_int(static_cast<int>(priority));
	print("\r\n", 2);
	char m6[] = "pc: ";
	uart_puts(0, 0, m6, sizeof(m6) - 1);
	print_int((uint64_t)pc);
	print("\r\n", 2);
	char m7[] = "sp: ";
	uart_puts(0, 0, m7, sizeof(m7) - 1);
	print_int((uint64_t)sp);
	print("\r\n", 2);
	char m8[] = "kernel_stack: ";
	uart_puts(0, 0, m8, sizeof(m8) - 1);
	print_int((uint64_t)kernel_stack);
	print("\r\n", 2);

	printf("state: %d\r\n", state);
}
