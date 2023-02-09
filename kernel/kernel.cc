#include "kernel.h"
#include "user/user_tasks.h"
#include "utils/printf.h"

int Task::Create(int priority, void (*function)()) {
	return to_kernel(Kernel::HandlerCode::CREATE, priority, function);
}
int Task::MyTid() {
	return to_kernel(Kernel::HandlerCode::MY_TID);
}
int Task::MyParentTid() {
	return to_kernel(Kernel::HandlerCode::MY_PARENT_ID);
}
void Task::Exit() {
	to_kernel(Kernel::HandlerCode::EXIT);
}
// potentially destroy in the future

void Task::Yield() {
	to_kernel(Kernel::HandlerCode::YIELD);
} // since it is more like a debug functin, it is consider as "else" namespace

int MessagePassing::Send::Send(int tid, const char* msg, int msglen, char* reply, int rplen) {
	return to_kernel(Kernel::HandlerCode::SEND, tid, msg, msglen, reply, rplen);
}

int MessagePassing::Receive::Receive(int* tid, char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::RECEIVE, tid, msg, msglen);
}

int MessagePassing::Reply::Reply(int tid, const char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::REPLY, tid, msg, msglen);
}

Kernel::Kernel() {
	allocate_new_task(Task::MAIDENLESS, 0, &UserTask::Task_test_4);
}

void Kernel::schedule_next_task() {
	active_task = scheduler.get_next();
	while (active_task == Task::NO_TASKS) {
		char m[] = "no tasks available...\r\n";
		uart_puts(0, 0, m, sizeof(m) - 1);
		for (int i = 0; i < 3000000; ++i)
			asm volatile("yield");
		active_task = scheduler.get_next();
	}
}

void Kernel::activate() {
	// upon activation, task become active
	active_request = tasks[active_task]->to_active();
}

Kernel::~Kernel() { }

void Kernel::handle() {
	HandlerCode request = (HandlerCode)active_request->x0; // x0 is always the request type

	if (request == HandlerCode::SEND) {
		handle_send();
	} else if (request == HandlerCode::RECEIVE) {
		handle_receive();
	} else if (request == HandlerCode::REPLY) {
		handle_reply();
	} else if (request == HandlerCode::CREATE) {
		int priority = active_request->x1;
		void (*user_task)() = (void (*)())active_request->x2;
		tasks[active_task]->to_ready(p_id_counter, &scheduler);
		// NOTE: allocate_new_task should be called at the end after everything is good
		allocate_new_task(tasks[active_task]->task_id, priority, user_task);
	} else if (request == HandlerCode::MY_TID) {
		tasks[active_task]->to_ready(tasks[active_task]->task_id, &scheduler);
	} else if (request == HandlerCode::MY_PARENT_ID) {
		tasks[active_task]->to_ready(tasks[active_task]->parent_id, &scheduler);
	} else if (request == HandlerCode::YIELD) {
		tasks[active_task]->to_ready(0x0, &scheduler);
	} else if (request == HandlerCode::EXIT) {
		tasks[active_task]->kill();
	}
}

void Kernel::allocate_new_task(int parent_id, int priority, void (*pc)()) {
	Descriptor::TaskDescriptor* task_ptr = task_allocator.get(p_id_counter, parent_id, priority, pc);
	if (task_ptr != nullptr) {
		tasks[p_id_counter] = task_ptr;
		scheduler.add_task(priority, p_id_counter);
		p_id_counter += 1;
	} else {
		char m1[] = "out of task space\r\n";
		uart_puts(0, 0, m1, sizeof(m1) - 1);
	}
}

void Kernel::handle_send() {
	int rid = active_request->x1;
	// I actually have no clue when will the -2 case trigger
	if (tasks[rid] == nullptr) {
		// communicating a non existing task
		tasks[active_task]->to_ready(MessagePassing::Send::Exception::NO_SUCH_TASK, &scheduler);
	} else {
		char* msg = (char*)active_request->x2;
		int msglen = active_request->x3;
		char* reply = (char*)active_request->x4;
		int replylen = active_request->x5;
		if (tasks[rid]->is_receive_block()) {
			tasks[rid]->fill_response(active_task, msg, msglen);
			tasks[rid]->to_ready(msglen, &scheduler);			 // unblock receiver, and the resonse is the length of the original message
			tasks[active_task]->to_reply_block(reply, replylen); // since you already put the message through, you just waiting on response
		} else {
			// reader is not ready to read we just push it to its inbox
			tasks[rid]->queue_message(active_task, msg, msglen);
			tasks[active_task]->to_send_block(reply, replylen); // you don't know who is going to send you shit
		}
	}
}

void Kernel::handle_receive() {
	int* from = (int*)active_request->x1;
	char* msg = (char*)active_request->x2;
	int msglen = active_request->x3;
	if (tasks[active_task]->have_message()) {
		Descriptor::Message incoming_msg = tasks[active_task]->pop_inbox();
		tasks[incoming_msg.from]->to_reply_block();
		tasks[active_task]->fill_message(incoming_msg, from, msg, msglen);
		tasks[active_task]->to_ready(incoming_msg.len, &scheduler);
	} else {
		// if we don't have message, you are put onto a receive block
		tasks[active_task]->to_receive_block(from, msg, msglen);
	}
}

void Kernel::handle_reply() {
	int to = active_request->x1;
	char* msg = (char*)active_request->x2;
	int msglen = active_request->x3;
	if (tasks[to] == nullptr) {
		tasks[active_task]->to_ready(MessagePassing::Reply::Exception::NO_SUCH_TASK, &scheduler); // communicating a non existing task
	} else if (!tasks[to]->is_reply_block()) {
		tasks[active_task]->to_ready(MessagePassing::Reply::Exception::NOT_WAITING_FOR_REPLY, &scheduler); // communicating with a task that is not reply blocked
	} else {
		int min_len = tasks[to]->fill_response(active_task, msg, msglen);
		tasks[to]->to_ready(min_len, &scheduler);
		tasks[active_task]->to_ready(min_len, &scheduler);
	}
}

int name_server_interface_helper(const char* name, Name::Iden iden) {
	char reply[4];
	const int rplen = sizeof(int);
	Name::NameServerReq req = { iden, { 0 } };

	for (uint64_t i = 0; name[i] != '\0' && i < Name::MAX_NAME_LENGTH; ++i)
		req.name.arr[i] = name[i];

	const int res = MessagePassing::Send::Send(Name::NAME_SERVER_ID, reinterpret_cast<const char*>(&req), Name::NAME_REQ_LENGTH, reply, rplen);
	if (res < 0) // Send failed
		return Name::Exception::INVALID_NS_TASK_ID;

	const int* r = reinterpret_cast<int*>(reply);
	return *r;
}

int Name::RegisterAs(const char* name) {
	const int ret = name_server_interface_helper(name, Name::Iden::REGISTER_AS);
	return (ret >= 0) ? 0 : Name::Exception::INVALID_NS_TASK_ID;
}

int Name::WhoIs(const char* name) {
	return name_server_interface_helper(name, Name::Iden::WHO_IS);
}
