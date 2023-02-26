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

void Task::_KernelPrint(const char* msg) {
	to_kernel(Kernel::HandlerCode::PRINT, msg);
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
	allocate_new_task(Task::MAIDENLESS, 0, &SystemTask::idle_task);
}

void Kernel::schedule_next_task() {
	int prev_task = active_task;
	active_task = scheduler.get_next();
	time_keeper.update_total_time(prev_task);
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
	if (active_task != SystemTask::IDLE_TID) {
		active_request = tasks[active_task]->to_active();
	} else {
		time_keeper.idle_start();
		active_request = tasks[active_task]->to_active();
		time_keeper.idle_end();
	}
}

Kernel::~Kernel() { }

void Kernel::handle() {
	KernelEntryCode kecode = static_cast<KernelEntryCode>(active_request->data);
#ifdef OUR_DEBUG
	printf("KEC: %llu\r\n", active_request->data);
#endif
	switch (kecode) {
	case KernelEntryCode::SYSCALL:
		handle_syscall();
		break;
	case KernelEntryCode::INTERRUPT: {
		/**
		 * The detail is that when you receive an interrupt, it doesn't mean there is only 1 interrupt happening
		 * You can have multiple interrupt, or maybe as you process this interrupt, a new interrupt come up
		 * to avoid excess context switching, we loop through all interrupt until it is completely cleared
		 */
		while (!Interrupt::is_interrupt_clear()) {
			tasks[active_task]->set_interrupted(true);
			uint32_t interrupt_id = Interrupt::get_interrupt_id();

			// Use mask to obtain the last 10 bits, see GICC_IAR spec
			InterruptCode icode = static_cast<InterruptCode>(interrupt_id & 0x3ff);
			handle_interrupt(icode);
			Interrupt::end_interrupt(interrupt_id);
		}

		break;
	}
	default:
		printf("Unknown kernel entry code: %d\r\n", kecode);
		while (true) {
		}
	}
}

void Kernel::handle_syscall() {
	HandlerCode request = (HandlerCode)active_request->x0; // x0 is always the request type

	switch (request) {
	case HandlerCode::SEND:
		handle_send();
		break;
	case HandlerCode::RECEIVE:
		handle_receive();
		break;
	case HandlerCode::REPLY:
		handle_reply();
		break;
	case HandlerCode::CREATE: {
		int priority = active_request->x1;
		void (*user_task)() = (void (*)())active_request->x2;
		tasks[active_task]->to_ready(p_id_counter, &scheduler);
		// NOTE: allocate_new_task should be called at the end after everything is good
		allocate_new_task(tasks[active_task]->task_id, priority, user_task);
		break;
	}
	case HandlerCode::MY_TID:
		tasks[active_task]->to_ready(tasks[active_task]->task_id, &scheduler);
		break;
	case HandlerCode::MY_PARENT_ID:
		tasks[active_task]->to_ready(tasks[active_task]->parent_id, &scheduler);
		break;
	case HandlerCode::YIELD:
		tasks[active_task]->to_ready(0x0, &scheduler);
		break;
	case HandlerCode::PRINT: {
		const char* msg = reinterpret_cast<const char*>(active_request->x1);
		printf(msg);
		tasks[active_task]->to_ready(0x0, &scheduler);
		break;
	}
	case HandlerCode::EXIT:
		tasks[active_task]->kill();
		break;
	case HandlerCode::AWAIT_EVENT: {
		int eventId = active_request->x1;
		handle_await_event(eventId);
		break;
	}
	case HandlerCode::AWAIT_EVENT_WITH_BUFFER: {
		int eventId = active_request->x1;
		char *buffer = (char*)active_request->x2;
		handle_await_event_with_buffer(eventId, buffer);
		break;
	}
	default:
		printf("Unknown syscall: %d from %d\r\n", request, active_task);
		uint64_t error_code = (read_esr() >> 26) & 0x3f;
		printf("ESR: %llx\r\n", error_code);
		while (true) {
		}
	}
}

void Kernel::handle_interrupt(InterruptCode icode) {
	switch (icode) {
	case InterruptCode::TIMER: {
		time_keeper.tick();

		if (clock_notifier_tid != Task::CLOCK_QUEUE_EMPTY) {
			tasks[clock_notifier_tid]->to_ready(0x0, &scheduler);
		} else {
			printf("Clock Too Slow \r\n");
			while (true) {
			}
		}
		break;
	}
	case InterruptCode::UART: {
		/**
		 * Note that no matter which interrupt, you receive from the same id, UART_INTERRUPT_ID
		 * this is kinda a problem, since a large variety of stuff can be from that type of interrupt,
		 * even same type, but uart0 vs uart1
		 *
		 * in order to differentiate them, we relies on checking the register later, IIR,
		 * IIR includes both the information about if there is an interrupt to handle, and if so, what is the interrupt exactly.
		 *
		 * the general work flow is 1. we receive UART_INTERRUPT_ID interrupt, thus recognize interrupt happened
		 * we check IIR to see what type of interrupt happened, we could potentially get a large quantity of interrupt
		 * overall, the goal is that if we receive interrupt in the form of UART_INTERRUPT_ID, we keep cleraing interrupt
		 * until every interrupt associated with uart is cleared
		 *
		 * Also note that server is in control of which register is flipped, thus also in control of which interrupt is happening.
		 */
		int exception_code = (int)(uart_get(0, 0, UART_IIR) & 0x3F);
		// this is a really shitty way to handle this, I think it would probably be better if we something similar to a dedicated class object
		// but we will fix it soon once experiementa go through.
		if (exception_code == UART::UART_RX_TIMEOUT && uart_0_receive_tid != Task::UART_0_RECEIVE_EMPTY) {
			int input_len = uart_get_all(0,0,tasks[uart_0_receive_tid]->get_event_buffer());
			tasks[uart_0_receive_tid]->to_ready(input_len, &scheduler);
		} else if (exception_code == UART::UART_TXR_INTERRUPT && uart_0_transmit_tid != Task::UART_0_TRANSMIT_FULL) {
			tasks[uart_0_transmit_tid]->to_ready(0x0, &scheduler);
		} else {
			printf("Uart Too Slow \r\n");
			while (true) {
			}
		}
		break;
	}
	default:
		printf("Unknown interrupt: %d\r\n", icode);
		while (true) {
		}
	}
	tasks[active_task]->to_ready(0x0, &scheduler);
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
		clock_notifier_tid = active_task;
		tasks[active_task]->to_event_block();
		break;
	}
	default:
		printf("Unknown event id: %d\r\n", eventId);
		break;
	}
}

void Kernel::handle_await_event_with_buffer(int eventId, char* buffer) {
	switch (eventId) {
	case UART::UART_RX_TIMEOUT: {
		uart_0_receive_tid = active_task;
		tasks[active_task]->to_event_block_with_buffer(buffer);
		break;
	}
	default:
		printf("Unknown event id: %d\r\n", eventId);
		break;
	}
}


void Kernel::start_timer() {
	time_keeper.start();
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

int Interrupt::AwaitEventWithBuffer(int eventId, char* buffer) {
	// this is a specialized version of await event, where we also accept a buffer which will be used to copy information
	// due to the natural size of event registers, it is assumed buffer holds at least 64 bytes
	return to_kernel(Kernel::HandlerCode::AWAIT_EVENT_WITH_BUFFER, eventId, buffer);
}