#include "kernel.h"
#include "interrupt/clock.h"
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
}

void Task::KernelPrint(const char* msg) {
	to_kernel(Kernel::HandlerCode::PRINT, msg);
}

void Task::LogTime(uint64_t time) {
	to_kernel(Kernel::HandlerCode::LOG_TIME, time);
}

int Message::Send::Send(int tid, const char* msg, int msglen, char* reply, int rplen) {
	return to_kernel(Kernel::HandlerCode::SEND, tid, msg, msglen, reply, rplen);
}

int Message::Receive::Receive(int* tid, char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::RECEIVE, tid, msg, msglen);
}

int Message::Reply::Reply(int tid, const char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::REPLY, tid, msg, msglen);
}

Kernel::Kernel() {
	allocate_new_task(Task::MAIDENLESS, 0, &UserTask::first_user_task);
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
	KernelEntryCode kecode = static_cast<KernelEntryCode>(active_request->data);
#ifdef OUR_DEBUG
	printf("Interrupt code: %llu\r\n", active_request->data);
#endif
	if (kecode == KernelEntryCode::SYSCALL) {
		// tasks[active_task]->set_interrupted(false);
		handle_syscall();
	} else if (kecode == KernelEntryCode::INTERRUPT) {
		// printf("Task %d interrupted!\r\n", active_task);
		tasks[active_task]->set_interrupted(true);
		uint32_t interrupt_id = Interrupt::get_interrupt_id();
		InterruptCode icode = static_cast<InterruptCode>(interrupt_id & 0x3ff);
		handle_interrupt(icode);
		Interrupt::end_interrupt(interrupt_id);
	} else {
		// wat
		printf("Unknown kernel entry code: %d\r\n", kecode);
		while (true) {
		}
	}
}

void Kernel::handle_syscall() {
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
	} else if (request == HandlerCode::PRINT) {
		const char* msg = reinterpret_cast<const char*>(active_request->x1);
		printf(msg);
		tasks[active_task]->to_ready(0x0, &scheduler);
	} else if (request == HandlerCode::LOG_TIME) {
		uint64_t time = active_request->x1;
		if (last_ping != 0) {
			total_time += time - last_ping;
		}

		last_ping = time;

		uint64_t leading = idle_time * 100 / total_time;
		uint64_t trailing = (idle_time * 100000) / total_time % 1000;

		// Save the cursor location, move to top left, print time, then restore cursor
		printf("\0337\x1b[HIdle time: %llu\r\nTotal time: %llu\r\n", idle_time, total_time);
		printf("Percentage: %llu.%03llu\0338", leading, trailing);

		tasks[active_task]->to_ready(0x0, &scheduler);
	} else if (request == HandlerCode::EXIT) {
		tasks[active_task]->kill();
	} else if (request == HandlerCode::AWAIT_EVENT) {
		int eventId = active_request->x1;
		handle_await_event(eventId);
	} else {
		printf("Unknown syscall: %d from %d\r\n", request, active_task);
		while (true) {
		}
	}
}

void Kernel::handle_interrupt(InterruptCode icode) {
	if (icode == InterruptCode::TIMER) {
		Clock::set_comparator(Clock::clo() + Clock::MICROS_PER_TICK);
		kernel_assert(clock_queue.size() == 1, "only clock notifier should be here");
		tasks[active_task]->to_ready(0x0, &scheduler);
		while (!clock_queue.empty()) {
			int tid = clock_queue.front();
			clock_queue.pop();
			tasks[tid]->to_ready(0x0, &scheduler);
		}

		// Manage some timing logic
		idle_time += Clock::time() - last_ping;

	} else {
		printf("Unknown interrupt: %d\r\n", icode);
		while (true) {
		}
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
		tasks[active_task]->to_ready(Message::Send::Exception::NO_SUCH_TASK, &scheduler);
	} else {
		char* msg = (char*)active_request->x2;
		int msglen = active_request->x3;
		char* reply = (char*)active_request->x4;
		int replylen = active_request->x5;
		if (tasks[rid]->is_receive_block()) {
			tasks[rid]->fill_response(active_task, msg, msglen);
			tasks[rid]->to_ready(msglen, &scheduler);			 // unblock receiver, and the response is the length of the original message
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
		tasks[active_task]->to_ready(Message::Reply::Exception::NO_SUCH_TASK, &scheduler); // communicating a non existing task
	} else if (!tasks[to]->is_reply_block()) {
		tasks[active_task]->to_ready(Message::Reply::Exception::NOT_WAITING_FOR_REPLY, &scheduler); // communicating with a task that is not reply blocked
	} else {
		int min_len = tasks[to]->fill_response(active_task, msg, msglen);
		tasks[to]->to_ready(min_len, &scheduler);
		tasks[active_task]->to_ready(min_len, &scheduler);
	}
}

void Kernel::handle_await_event(int eventId) {
	switch (eventId) {
	case Clock::TIMER_INTERRUPT_ID: {
		clock_queue.push(active_task);
		tasks[active_task]->to_event_block();
		break;
	}
	default:
		printf("Unknown event id: %d\r\n", eventId);
		break;
	}
}

int name_server_interface_helper(const char* name, Name::RequestHeader header) {
	char reply[4];
	const int rplen = sizeof(int);
	Name::NameServerReq req = { header, { 0 } };

	for (uint64_t i = 0; name[i] != '\0' && i < Name::MAX_NAME_LENGTH; ++i)
		req.name.arr[i] = name[i];

	const int res = Message::Send::Send(Name::NAME_SERVER_ID, reinterpret_cast<const char*>(&req), Name::NAME_REQ_LENGTH, reply, rplen);
	if (res < 0) // Send failed
		return Name::Exception::INVALID_NS_TASK_ID;

	const int* r = reinterpret_cast<int*>(reply);
	return *r;
}

int Name::RegisterAs(const char* name) {
	const int ret = name_server_interface_helper(name, Name::RequestHeader::REGISTER_AS);
	return (ret >= 0) ? 0 : Name::Exception::INVALID_NS_TASK_ID;
}

int Name::WhoIs(const char* name) {
	return name_server_interface_helper(name, Name::RequestHeader::WHO_IS);
}

int timer_server_interface_helper(int tid, Clock::RequestHeader header, uint32_t ticks = 0) {
	char reply[4];
	Clock::ClockServerReq req = { header, { ticks } }; // body is irrelevant
	if (tid != Clock::CLOCK_SERVER_ID) {
		return Clock::Exception::INVALID_ID;
	}
#ifdef OUR_DEBUG
	const int res = Message::Send::Send(tid, reinterpret_cast<const char*>(&req), sizeof(Clock::ClockServerReq), reply, 4);
	if (res < 0) // Send failed
		return Name::Exception::INVALID_NS_TASK_ID;
#else
	Message::Send::Send(tid, reinterpret_cast<const char*>(&req), sizeof(Clock::ClockServerReq), reply, 4);
#endif
	return *(reinterpret_cast<int*>(reply)); // return the number of ticks since the dawn of time
}

int Clock::Time(int tid) {
	return timer_server_interface_helper(tid, Clock::RequestHeader::TIME);
}

int Clock::Delay(int tid, int ticks) {
	if (ticks < 0) {
		return Clock::Exception::NEGATIVE_DELAY;
	}
	return timer_server_interface_helper(tid, Clock::RequestHeader::DELAY, (uint32_t)ticks);
}

int Clock::DelayUntil(int tid, int ticks) {
	if (ticks < 0) {
		return Clock::Exception::NEGATIVE_DELAY;
	}
	return timer_server_interface_helper(tid, Clock::RequestHeader::DELAY_UNTIL, (uint32_t)ticks);
}

int Interrupt::AwaitEvent(int eventId) {
	return to_kernel(Kernel::HandlerCode::AWAIT_EVENT, eventId);
}
