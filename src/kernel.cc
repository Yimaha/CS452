#include "kernel.h"
#include "interrupt/clock.h"
#include "user/user_tasks.h"
#include "utils/printf.h"

using namespace Message;

int Task::Create(Priority priority, void (*function)()) {
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

Priority Task::MyPriority() {
	return static_cast<Priority>(to_kernel(Kernel::HandlerCode::MY_PRIORITY));
}

int Message::Send::Send(int tid, const char* msg, int msglen, char* reply, int rplen) {
	return to_kernel(Kernel::HandlerCode::SEND, tid, msg, msglen, reply, rplen);
}

int Message::Send::SendNoReply(int tid, const char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::SEND, tid, msg, msglen, nullptr, 0);
}

int Message::Send::EmptySend(int tid) {
	return to_kernel(Kernel::HandlerCode::SEND, tid, nullptr, 0, nullptr, 0);
}

int Message::Receive::Receive(int* tid, char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::RECEIVE, tid, msg, msglen);
}

int Message::Receive::EmptyReceive(int* tid) {
	return to_kernel(Kernel::HandlerCode::RECEIVE, tid, nullptr, 0);
}

int Message::Reply::Reply(int tid, const char* msg, int msglen) {
	return to_kernel(Kernel::HandlerCode::REPLY, tid, msg, msglen);
}

int Message::Reply::EmptyReply(int tid) {
	return to_kernel(Kernel::HandlerCode::REPLY, tid, nullptr, 0);
}

int name_server_interface_helper(const char* name, Message::RequestHeader header) {
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
	const int ret = name_server_interface_helper(name, Message::RequestHeader::REGISTER_AS);
	return (ret >= 0) ? 0 : Name::Exception::INVALID_NS_TASK_ID;
}

int Name::WhoIs(const char* name) {
	return name_server_interface_helper(name, Message::RequestHeader::WHO_IS);
}

int timer_server_interface_helper(int tid, Message::RequestHeader header, uint32_t ticks = 0) {
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
	return timer_server_interface_helper(tid, Message::RequestHeader::TIME);
}

int Clock::Delay(int tid, int ticks) {
	if (ticks < 0) {
		return Clock::Exception::NEGATIVE_DELAY;
	}
	return timer_server_interface_helper(tid, Message::RequestHeader::DELAY, (uint32_t)ticks);
}

int Clock::DelayUntil(int tid, int ticks) {
	if (ticks < 0) {
		return Clock::Exception::NEGATIVE_DELAY;
	}
	return timer_server_interface_helper(tid, Message::RequestHeader::DELAY_UNTIL, (uint32_t)ticks);
}

int Clock::IdleStats(uint64_t* idle_time, uint64_t* total_time) {
	return to_kernel(Kernel::HandlerCode::IDLE_STATS, idle_time, total_time);
}

int Interrupt::AwaitEvent(int eventId) {
	return to_kernel(Kernel::HandlerCode::AWAIT_EVENT, eventId);
}

int Interrupt::AwaitEventWithBuffer(int eventId, char* buffer) {
	// this is a specialized version of await event, where we also accept a buffer which will be used to copy
	// information due to the natural size of event registers, it is assumed buffer holds at least 64 bytes note the
	// return value is how much of the buffer is used
	return to_kernel(Kernel::HandlerCode::AWAIT_EVENT_WITH_BUFFER, eventId, buffer);
}

int UART::UartWriteRegister(int channel, char reg, char data) {
	return to_kernel(Kernel::HandlerCode::WRITE_REGISTER, channel, reg, data);
}

int UART::UartReadRegister(int channel, char reg) {
	return to_kernel(Kernel::HandlerCode::READ_REGISTER, channel, reg);
}

int UART::TransInterrupt(int channel, bool enable) {
	return to_kernel(Kernel::HandlerCode::TRANSMIT_INTERRUPT, channel, enable);
}

int UART::ReceiveInterrupt(int channel, bool enable) {
	return to_kernel(Kernel::HandlerCode::RECEIVE_INTERRUPT, channel, enable);
}

int UART::UartReadAll(int channel, char* buffer) { // designed for reading all the bytes out of UART_RHR
	return to_kernel(Kernel::HandlerCode::READ_ALL, channel, buffer);
}

int UART::Putc(int tid, int uart, char ch) {
	// since we only have uart0, uart param is ignored
	if ((uart == 0 && tid != UART::UART_0_TRANSMITTER_TID) || (uart == 1 && tid != UART::UART_1_TRANSMITTER_TID)) {
		Task::_KernelCrash("either id is not correct in Putc\r\n");
	}
	UART::UARTServerReq req = UART::UARTServerReq(RequestHeader::UART_PUTC, ch);
	Message::Send::SendNoReply(tid, reinterpret_cast<const char*>(&req), sizeof(UART::UARTServerReq));
	return 0;
}

int UART::Puts(int tid, int uart, const char* s, uint64_t len) {
	// since we only have uart0, uart param is ignored
	if ((uart == 0 && tid != UART::UART_0_TRANSMITTER_TID) || (uart == 1 && tid != UART::UART_1_TRANSMITTER_TID)) {
		
		Task::_KernelCrash("%d: id is not correct Puts\r\n", Task::MyTid());
	} else if (len >= UART::UART_MESSAGE_LIMIT) {
		Task::_KernelCrash("%d: len is too big in Puts\r\n", Task::MyTid());
	}

	UART::WorkerRequestBody body;
	body.msg_len = len;
	for (uint64_t i = 0; i < len; i++) {
		body.msg[i] = s[i];
	}

	UART::UARTServerReq req = UART::UARTServerReq(RequestHeader::UART_PUTS, body);
	Message::Send::SendNoReply(tid, reinterpret_cast<const char*>(&req), sizeof(UART::UARTServerReq));
	return 0;
}

int UART::PutsNullTerm(int tid, int uart, const char* s, uint64_t len) {
	// since we only have uart0, uart param is ignored
	if ((uart == 0 && tid != UART::UART_0_TRANSMITTER_TID) || (uart == 1 && tid != UART::UART_1_TRANSMITTER_TID)) {
		return -1;
	}
	UART::WorkerRequestBody body;
	for (body.msg_len = 0; body.msg_len < len && (s[body.msg_len] != '\0'); body.msg_len++) {
		body.msg[body.msg_len] = s[body.msg_len];
	}

	UART::UARTServerReq req = UART::UARTServerReq(RequestHeader::UART_PUTS, body);
	Message::Send::SendNoReply(tid, reinterpret_cast<const char*>(&req), sizeof(UART::UARTServerReq));
	return 0;
}

int UART::Getc(int tid, int uart) {
	// since we only have uart0, uart param is ignored
	if ((uart == 0 && tid != UART::UART_0_RECEIVER_TID) || (uart == 1 && tid != UART::UART_1_RECEIVER_TID)) {
		return -1;
	}
	UART::UARTServerReq req = UART::UARTServerReq(RequestHeader::UART_GETC, '0'); // body is irrelevant
	char c;
	Message::Send::Send(tid, reinterpret_cast<const char*>(&req), sizeof(UART::UARTServerReq), &c, 1);
	return (int)c;
}

Kernel::Kernel() {
	allocate_new_task(Task::MAIDENLESS, Priority::LAUNCH_PRIORITY, &UserTask::launch);
}

void Kernel::schedule_next_task() {
	active_task = scheduler.get_next();
	time_keeper.update_total_time();
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
		tasks[active_task]->to_interrupted(&scheduler);
		uint32_t interrupt_id = Interrupt::get_interrupt_id();
		// Use mask to obtain the last 10 bits, see GICC_IAR spec
		InterruptCode icode = static_cast<InterruptCode>(interrupt_id & 0x3ff);
		do {
			handle_interrupt(icode);
			Interrupt::end_interrupt(interrupt_id);
			interrupt_id = Interrupt::get_interrupt_id();
			// Use mask to obtain the last 10 bits, see GICC_IAR spec
			icode = static_cast<InterruptCode>(interrupt_id & 0x3ff);
		} while (icode != InterruptCode::CLEAR);
		break;
	}
	default:
		kcrash("Unknown kernel entry code: %d\r\n", kecode);
	}
}

void Kernel::handle_syscall() {
	HandlerCode request = (HandlerCode)active_request->x0; // x0 is always the request type

#ifdef OUR_DEBUG
	KernelEntryInfo keinfo = KernelEntryInfo(active_task, request, active_request->x1, active_request->x2);
	backtrace_stack.push(keinfo);
#endif

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
	case HandlerCode::CREATE:
		handle_create();
		break;
	case HandlerCode::MY_TID:
		tasks[active_task]->to_ready(tasks[active_task]->task_id, &scheduler);
		break;
	case HandlerCode::MY_PARENT_ID:
		tasks[active_task]->to_ready(tasks[active_task]->parent_id, &scheduler);
		break;
	case HandlerCode::YIELD:
		tasks[active_task]->to_ready(0x0, &scheduler);
		break;
	case HandlerCode::EXIT:
		tasks[active_task]->kill();
		break;
	case HandlerCode::MY_PRIORITY:
		tasks[active_task]->to_ready(static_cast<int>(tasks[active_task]->priority), &scheduler);
		break;
	case HandlerCode::AWAIT_EVENT:
		handle_await_event((int)active_request->x1);
		break;
	case HandlerCode::AWAIT_EVENT_WITH_BUFFER:
		handle_await_event_with_buffer(active_request->x1, (char*)active_request->x2);
		break;
	case HandlerCode::WRITE_REGISTER:
		handle_write_register();
		break;
	case HandlerCode::READ_REGISTER:
		handle_read_register();
		break;
	case HandlerCode::READ_ALL:
		handle_read_all();
		break;
	case HandlerCode::TRANSMIT_INTERRUPT:
		handle_transmit_interrupt();
		break;
	case HandlerCode::RECEIVE_INTERRUPT:
		handle_receive_interrupt();
		break;
	case HandlerCode::IDLE_STATS:
		handle_idle_stats();
		break;
	case HandlerCode::CRASH: {
		const char* msg = reinterpret_cast<const char*>(active_request->x1);
		kcrash(msg);
		break;
	}
	default:
		printf("\r\nUnknown syscall: %d from %d\r\n", request, active_task);
		uint64_t error_code = (read_esr() >> 26) & 0x3f;
		kcrash("ESR: %llx\r\n", error_code);
		break;
	}
}

void Kernel::interrupt_control(int channel) {
	int control = UART::get_control_bits(enable_transmit_interrupt[channel], enable_receive_interrupt[channel], enable_CTS[channel]);
	uart_put(UART::SPI_CHANNEL, channel, UART_IER, control);
}

void Kernel::handle_interrupt(InterruptCode icode) {

#ifdef OUR_DEBUG
	KernelEntryInfo keinfo = KernelEntryInfo(active_task, HandlerCode::NONE, 0, 0, icode);
	backtrace_stack.push(keinfo);
#endif

	switch (icode) {
	case InterruptCode::TIMER: {
		time_keeper.tick();

		if (clock_notifier_tid != Task::MAIDENLESS) {
			tasks[clock_notifier_tid]->to_ready(0x0, &scheduler);
		} else {
			kcrash("Clock Too Slow \r\n");
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
		 * IIR includes both the information about if there is an interrupt to handle, and if so, what is the interrupt
		 * exactly.
		 *
		 * the general work flow is 1. we receive UART_INTERRUPT_ID interrupt, thus recognize interrupt happened
		 * we check IIR to see what type of interrupt happened, we could potentially get a large quantity of interrupt
		 * overall, the goal is that if we receive interrupt in the form of UART_INTERRUPT_ID, we keep cleraing
		 * interrupt until every interrupt associated with uart is cleared
		 *
		 * Also note that server is in control of which register is flipped, thus also in control of which interrupt is
		 * happening.
		 */

		int exception_code = (int)(uart_get(DEFAULT_SPI_CHANNEL, TERMINAL_UART_CHANNEL, UART_IIR) & 0x3F);

		do {
			// this is a really shitty way to handle this, I think it would probably be better if we something similar
			// to a dedicated class object but we will fix it soon once experiementa go through.
			if (exception_code == UART::InterruptType::UART_RX_TIMEOUT && uart_0_receive_tid != Task::MAIDENLESS) {
				int input_len = uart_get_all(DEFAULT_SPI_CHANNEL, TERMINAL_UART_CHANNEL, tasks[uart_0_receive_tid]->get_event_buffer());
				tasks[uart_0_receive_tid]->to_ready(input_len, &scheduler);
				uart_0_receive_tid = Task::MAIDENLESS;
				enable_receive_interrupt[TERMINAL_UART_CHANNEL] = false;
				interrupt_control(TERMINAL_UART_CHANNEL);
			} else if (exception_code == UART::InterruptType::UART_TXR_INTERRUPT && uart_0_transmit_tid != Task::MAIDENLESS) {
				tasks[uart_0_transmit_tid]->to_ready(0x0, &scheduler);
				uart_0_transmit_tid = Task::MAIDENLESS;
				enable_transmit_interrupt[TERMINAL_UART_CHANNEL] = false;
				interrupt_control(TERMINAL_UART_CHANNEL);
			} else if (exception_code == UART::InterruptType::UART_CLEAR) {
				break;
			} else {
				kcrash("Uart 0 Too Slow \r\nexception code: %d receive_tid: %d transmit_tid %d\r\n",
					   exception_code,
					   uart_0_receive_tid,
					   uart_0_transmit_tid);
			}
			exception_code = (int)(uart_get(DEFAULT_SPI_CHANNEL, TERMINAL_UART_CHANNEL, UART_IIR) & 0x3F);
		} while (exception_code != UART::InterruptType::UART_CLEAR);

		exception_code = (int)(uart_get(DEFAULT_SPI_CHANNEL, TRAIN_UART_CHANNEL, UART_IIR) & 0x3F);
		do {

			// this is a really shitty way to handle this, I think it would probably be better if we something similar
			// to a dedicated class object but we will fix it soon once experiementa go through.
			if (exception_code == UART::InterruptType::UART_RX_TIMEOUT && uart_1_receive_timeout_tid != Task::MAIDENLESS) {
				tasks[uart_1_receive_timeout_tid]->to_ready(0x0, &scheduler);
				uart_1_receive_timeout_tid = Task::MAIDENLESS;
				enable_receive_interrupt[TRAIN_UART_CHANNEL] = false;
				interrupt_control(TRAIN_UART_CHANNEL);
			} else if (exception_code == UART::InterruptType::UART_RX_INTERRUPT && uart_1_receive_tid != Task::MAIDENLESS) {
				tasks[uart_1_receive_tid]->to_ready(0x0, &scheduler);
				uart_1_receive_tid = Task::MAIDENLESS;
				enable_receive_interrupt[TRAIN_UART_CHANNEL] = false;
				interrupt_control(TRAIN_UART_CHANNEL);
			} else if (exception_code == UART::InterruptType::UART_MODEM_INTERRUPT && uart_1_msr_tid != Task::MAIDENLESS) {
				char state = uart_get(DEFAULT_SPI_CHANNEL, TRAIN_UART_CHANNEL, UART_MSR);
				if ((state & 0x1) == 0x1 && (state & 0b10000) == 0b10000) {
					tasks[uart_1_msr_tid]->to_ready(0x0, &scheduler);
					uart_1_msr_tid = Task::MAIDENLESS;
				}
			} else if (exception_code == UART::InterruptType::UART_TXR_INTERRUPT && uart_1_transmit_tid != Task::MAIDENLESS) {
				tasks[uart_1_transmit_tid]->to_ready(0x0, &scheduler);
				uart_1_transmit_tid = Task::MAIDENLESS;
				enable_transmit_interrupt[TRAIN_UART_CHANNEL] = false;
				interrupt_control(TRAIN_UART_CHANNEL);
			} else if (exception_code == UART::InterruptType::UART_CLEAR) {
				break;
			} else {
				kcrash("Uart 1 Too Slow \r\nexception code: %d receive_tid: %d transmit_tid %d msr_tid %d\r\n",
					   exception_code,
					   uart_1_receive_tid,
					   uart_1_transmit_tid,
					   uart_1_msr_tid);
			}
			exception_code = (int)(uart_get(DEFAULT_SPI_CHANNEL, TRAIN_UART_CHANNEL, UART_IIR) & 0x3F);
		} while (exception_code != UART::InterruptType::UART_CLEAR);
		UART::clear_uart_interrupt();
		break;
	}
	default:
		kcrash("Unknown interrupt code: %d\r\n", icode);
	}
}

void Kernel::allocate_new_task(int parent_id, Priority priority, void (*pc)()) {
	Descriptor::TaskDescriptor* task_ptr = task_allocator.get(p_id_counter, parent_id, priority, pc);
	if (task_ptr != nullptr) {
		tasks[p_id_counter] = task_ptr;
		scheduler.add_task(priority, p_id_counter);
		p_id_counter += 1;
	} else {
		// this need to cause crash
		kcrash("out of task space, all tasks are allocated\r\n");
	}
}

void Kernel::handle_create() {
	Priority priority = static_cast<Priority>(active_request->x1);
	void (*user_task)() = (void (*)())active_request->x2;
	tasks[active_task]->to_ready(p_id_counter, &scheduler);
	// NOTE: allocate_new_task should be called at the end after everything is good
	allocate_new_task(tasks[active_task]->task_id, priority, user_task);
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
		Descriptor::MessageStruct incoming_msg = tasks[active_task]->pop_inbox();
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
		tasks[active_task]->to_ready(Message::Reply::Exception::NO_SUCH_TASK,
									 &scheduler); // communicating a non existing task
	} else if (!tasks[to]->is_reply_block()) {
		tasks[active_task]->to_ready(Message::Reply::Exception::NOT_WAITING_FOR_REPLY,
									 &scheduler); // communicating with a task that is not reply blocked
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
	case UART::InterruptEvents::UART_0_TXR_INTERRUPT: {
		uart_0_transmit_tid = active_task;
		tasks[active_task]->to_event_block();
		break;
	}
	case UART::InterruptEvents::UART_1_TXR_INTERRUPT: {
		uart_1_transmit_tid = active_task;
		tasks[active_task]->to_event_block();
		break;
	}
	case UART::InterruptEvents::UART_1_MSR_INTERRUPT: {
		uart_1_msr_tid = active_task;
		tasks[active_task]->to_event_block();
		break;
	}
	case UART::InterruptEvents::UART_1_RX_INTERRUPT: {
		uart_1_receive_tid = active_task;
		tasks[active_task]->to_event_block();
		break;
	}
	case UART::InterruptEvents::UART_1_RX_TIMEOUT: {
		uart_1_receive_timeout_tid = active_task;
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
	case UART::InterruptEvents::UART_0_RX_TIMEOUT: {
		uart_0_receive_tid = active_task;
		tasks[active_task]->to_event_block_with_buffer(buffer);
		break;
	}
	default:
		printf("Unknown event id: %d\r\n", eventId);
		break;
	}
}

void Kernel::handle_write_register() {
	int channel = active_request->x1;
	char reg = active_request->x2;
	char data = active_request->x3;

	bool success = true;
	if (reg == UART_THR) {
		success = uart_putc_non_blocking(UART::SPI_CHANNEL, channel, data);
	} else {
		uart_put(UART::SPI_CHANNEL, channel, reg, data);
	}
	tasks[active_task]->to_ready((success ? UART::SUCCESSFUL : UART::Exception::FAILED_TO_WRITE), &scheduler);
}

void Kernel::handle_read_register() {
	int channel = active_request->x1;
	char reg = active_request->x2;

	bool success = true;
	char c;
	if (reg == UART_RHR) {
		success = uart_getc_non_blocking(UART::SPI_CHANNEL, channel, &c);
	} else {
		c = uart_get(UART::SPI_CHANNEL, channel, reg);
	}
	tasks[active_task]->to_ready((success ? (int)c : UART::Exception::FAILED_TO_READ), &scheduler);
}

void Kernel::handle_read_all() {
	int channel = active_request->x1;
	char* buffer = (char*)active_request->x2;
	int len = uart_get_all(UART::SPI_CHANNEL, channel, buffer);
	tasks[active_task]->to_ready(len, &scheduler);
}

void Kernel::handle_transmit_interrupt() {
	int channel = active_request->x1;
	bool enable = active_request->x2;
	enable_transmit_interrupt[channel] = enable;
	interrupt_control(channel);
	tasks[active_task]->to_ready(0x0, &scheduler);
}

void Kernel::handle_receive_interrupt() {
	int channel = active_request->x1;
	bool enable = active_request->x2;
	enable_receive_interrupt[channel] = enable;
	interrupt_control(channel);
	tasks[active_task]->to_ready(0x0, &scheduler);
}

void Kernel::handle_idle_stats() {
	uint64_t* idle = reinterpret_cast<uint64_t*>(active_request->x1);
	uint64_t* total = reinterpret_cast<uint64_t*>(active_request->x2);

	*idle = time_keeper.get_idle_time();
	*total = time_keeper.get_total_time();
	tasks[active_task]->to_ready(0x0, &scheduler);
}

void Kernel::start_timer() {
	time_keeper.start();
}
