

#pragma once

#include <new>
#include <stdint.h>

#include "context_switch.h"
#include "descriptor.h"
#include "etl/circular_buffer.h"
#include "interrupt_handler.h"
#include "k1/user_tasks_k1.h"
#include "k2/user_tasks_k2.h"
#include "k2/user_tasks_k2_performance.h"
#include "rpi.h"
#include "scheduler.h"
#include "server/clock_server.h"
#include "server/name_server.h"
#include "server/terminal_admin.h"
#include "server/uart_server.h"
#include "user/idle_task.h"
#include "utils/slab_allocator.h"

namespace Task
{
constexpr int MAIDENLESS = -1;
constexpr uint64_t USER_TASK_START_ADDRESS = 0x10000000;
constexpr uint64_t USER_TASK_LIMIT
	= SCHEDULER_QUEUE_SIZE; // We exactly how much task we are going to create, thus, we can afford to a large quantity of User Task

int MyTid();
int MyParentTid();
void Exit();
void Yield();
int Create(Priority priority, void (*function)());
Priority MyPriority();

const int MAX_CRASH_MSG_LEN = 256;

// Crash function, with format string argument
template <typename... Args>
void _KernelCrash(const char* msg, Args... args);

}

/**
 * User communication interface, all user tasks should use these functions to talk to the kernel
 * to signify the importance as these all trigger context switches,
 * note that they follow Capitalized Camel Case
 * unlike all other functions within our code
 */
namespace Message
{
namespace Send
{
	int Send(int tid, const char* msg, int msglen, char* reply, int rplen);
	int SendNoReply(int tid, const char* msg, int msglen);
	int EmptySend(int tid);
	enum Exception { NO_SUCH_TASK = -1, CANNOT_BE_COMPLETE = -2 };
}

namespace Receive
{
	int Receive(int* tid, char* msg, int msglen);
	int EmptyReceive(int* tid);
}

namespace Reply
{
	int Reply(int tid, const char* msg, int msglen);
	int EmptyReply(int tid);
	enum Exception { NO_SUCH_TASK = -1, NOT_WAITING_FOR_REPLY = -2 };
}
}

/* Name server namespace
 * A couple notes about the RegisterAs and WhoIs functions:
 * 1. The name must be a null terminated string or less than MAX_NAME_LENGTH = 15
 * 2. The name must be unique, if you try to register a name that is already registered,
 * 		it will return an error
 * 3. The name server is always task 1
 * 4. The name server uses a hashmap to map names to tids, however the hashmap
 * 		only uses the first 8 bytes of the name, so if you have two names that
 * 		are the same up to the first 8 bytes, there will be potentially slow hash collisions
 */
namespace Name
{
const uint64_t NAME_SERVER_ID = 1; // the name server is always task 1

//*************************************************************************
/// Registers the given name with the name server.
///\return 0 on success, or -1 if the name server task id is invalid
//*************************************************************************
int RegisterAs(const char* name);

//*************************************************************************
/// Gets the tid of the task with the given name.
///\return The tid of the task with the given name, or:
// 	-1 if the name server task id is invalid
// 	-2 if the name is not registered
//*************************************************************************
int WhoIs(const char* name);
}

namespace Clock
{
const uint64_t CLOCK_SERVER_ID = 2;
const uint64_t CLOCK_NOTIFIER_ID = 3;
const uint32_t CLOCK_QUEUE_LENGTH = 64;

//*****************************************************************************
/// Gets the current time in ticks, where a tick is 10ms.
///\return The current time in ticks, or -1 if the clock server tid is invalid.
//*****************************************************************************
int Time(int tid);

//*************************************************************************
/// Delays the current task for the given number of ticks.
///\return The current time in ticks, or
// 	-1 if the clock server tid is invalid, or
// 	-2 if the delay is negative.
//*************************************************************************
int Delay(int tid, int ticks);

//*************************************************************************
/// Delays the current task until the given time.
///\return The current time in ticks, or
// 	-1 if the clock server tid is invalid, or
// 	-2 if the delay is negative.
//*************************************************************************
int DelayUntil(int tid, int ticks);

//*************************************************************************
/// Gets the current idle time and total time in microseconds,
/// and stores them in the given pointers.
///\return 0 on success, or -1 if there are errors for some reason.
//*************************************************************************
int IdleStats(uint64_t* idle_time, uint64_t* total_time);
}

namespace Interrupt
{
int AwaitEvent(int eventid);
int AwaitEventWithBuffer(int eventId, char* buffer);
}

namespace UART
{
int UartWriteRegister(int channel, char reg, char data);
int UartReadRegister(int channel, char reg);
int Putc(int tid, int uart, char ch);
int Puts(int tid, int uart, const char* s, uint64_t len);
int PutsNullTerm(int tid, int uart, const char* s, uint64_t len);
int Getc(int tid, int uart);
int TransInterrupt(int channel, bool enable);
int ReceiveInterrupt(int channel, bool enable);
int UartReadAll(int channel, char* buffer);
const int SPI_CHANNEL = 0;
const int SUCCESSFUL = 0;
enum Exception { INVALID_SERVER_TASK = -1, FAILED_TO_WRITE = -2, FAILED_TO_READ = -3 };

}

/**
 * Kernel state class, stores important information about the kernel and control the flow
 * */
class Kernel {
public:
	enum HandlerCode {
		NONE = 0,
		CREATE = 1,
		MY_TID = 2,
		MY_PARENT_ID = 3,
		YIELD = 4,
		EXIT = 5,
		SEND = 6,
		RECEIVE = 7,
		REPLY = 8,
		REGISTER_AS = 9,
		WHO_IS = 10,
		TIME = 11,
		DELAY = 12,
		DELAY_UNTIL = 13,
		AWAIT_EVENT = 14,
		AWAIT_EVENT_WITH_BUFFER = 15,
		CRASH = 16,
		WRITE_REGISTER = 17,
		READ_REGISTER = 18,
		READ_ALL = 19,
		TRANSMIT_INTERRUPT = 20,
		RECEIVE_INTERRUPT = 21,
		IDLE_STATS = 22,
		MY_PRIORITY = 23,
	};

	enum KernelEntryCode { SYSCALL = 0, INTERRUPT = 1 };
	enum InterruptCode { NA = 0, TIMER = Clock::TIMER_INTERRUPT_ID, UART = UART::UART_INTERRUPT_ID, CLEAR = 1023 };

	Kernel();
	~Kernel();
	void schedule_next_task();
	void activate();
	void handle();
	void handle_syscall();
	void handle_interrupt(InterruptCode icode);
	void start_timer();

	template <typename... Args>
	void kcrash(const char* msg, Args... args);

private:
	// static is needed to define value at compile time
	static const uint64_t BACK_TRACE_SIZE = 512;
	static const int TERMINAL_UART_CHANNEL = 0;
	static const int TRAIN_UART_CHANNEL = 1;
	static const int DEFAULT_SPI_CHANNEL = 0;

	int p_id_counter = 0;					  // keeps track of new task creation id
	int active_task = 0;					  // keeps track of the active_task id
	InterruptFrame* active_request = nullptr; // a storage that saves the active user request
	Task::Scheduler scheduler;				  // scheduler doesn't hold the actual task descriptor,
											  // simply an id and the priority

	Descriptor::TaskDescriptor* tasks[Task::USER_TASK_LIMIT] = { nullptr }; // points to the starting location of taskDescriptors, default all nullptr

	// define the type, and follow by the constructor variable you want to pass to i
	SlabAllocator<Descriptor::TaskDescriptor, int, int, Priority, void (*)()> task_allocator
		= SlabAllocator<Descriptor::TaskDescriptor, int, int, Priority, void (*)()>((char*)Task::USER_TASK_START_ADDRESS, Task::USER_TASK_LIMIT);

	Clock::TimeKeeper time_keeper = Clock::TimeKeeper();

	/*
	 * Struct that represents the information contained in a kernel entry
	 * Used to keep a backtrace stack
	 */
	struct KernelEntryInfo {
		int tid;
		HandlerCode handler_code;
		uint64_t arg1;
		uint64_t arg2;

		InterruptCode icode;
		KernelEntryInfo(int tid, HandlerCode handler_code, int arg1, int arg2, InterruptCode icode = InterruptCode::NA)
			: tid(tid)
			, handler_code(handler_code)
			, arg1(arg1)
			, arg2(arg2)
			, icode(icode) { }
	};

	// Backtrace stack
	etl::circular_buffer<KernelEntryInfo, BACK_TRACE_SIZE> backtrace_stack = etl::circular_buffer<KernelEntryInfo, BACK_TRACE_SIZE>();
	// list of interrupt related parking log
	// note that fail to handle interrupt means death, and we only have 1 parking spot for each type
	// clock notifier "list", a pointer to the notifier
	int clock_notifier_tid = Task::MAIDENLESS; // always 1 agent for time

	int uart_0_receive_tid = Task::MAIDENLESS;	// always 1 agent for receiving
	int uart_0_transmit_tid = Task::MAIDENLESS; // always 1 agent for transmitting

	int uart_1_receive_tid = Task::MAIDENLESS;		   // always 1 agent for receiving
	int uart_1_receive_timeout_tid = Task::MAIDENLESS; // always 1 agent for transmitting

	int uart_1_transmit_tid = Task::MAIDENLESS;
	int uart_1_msr_tid = Task::MAIDENLESS;

	bool enable_transmit_interrupt[2] = { false, false };
	bool enable_receive_interrupt[2] = { false, false };
	bool enable_CTS[2] = { false, true };

	void allocate_new_task(int parent_id, Priority priority,
						   void (*pc)()); // create, and push a new task onto the actual scheduler
	void handle_create();
	void handle_send();
	void handle_receive();
	void handle_reply();
	void handle_await_event(int eventId);
	void handle_await_event_with_buffer(int eventId, char* buffer);
	void handle_write_register();
	void handle_read_register();
	void handle_read_all();
	void handle_transmit_interrupt();
	void handle_receive_interrupt();
	void handle_idle_stats();
	void interrupt_control(int channel);
};

// Has to be defined after the Kernel class because it depends on Handler codes
namespace Task
{
template <typename... Args>
void _KernelCrash(const char* msg, Args... args) {
	char buf[MAX_CRASH_MSG_LEN];
	snprintf(buf, MAX_CRASH_MSG_LEN, msg, args...);
	to_kernel(Kernel::HandlerCode::CRASH, buf);
}
}

// Has to be defined after the Kernel class because it depends on the Kernel class
template <typename... Args>
void Kernel::kcrash(const char* msg, Args... args) {
	printf(msg, args...);
	for (auto& keinfo : backtrace_stack) {
		printf("Task [%d], HandlerCode %d, Arg1 %d, Arg2 %d, ICode %d\r\n", keinfo.tid, keinfo.handler_code, keinfo.arg1, keinfo.arg2, keinfo.icode);
	}

	while (true) {
	}
}

template <typename... Args>
void debug_print(int uart_tid, const char* msg, Args... args) {
	char buf[300];
	int len = snprintf(buf, 300, msg, args...);
	UART::Puts(uart_tid, 0, buf, len);
}

// template <typename... Args>
// void debug_print(const char* msg, Args... args) {
// 	char buf[128];
// 	snprintf(buf, 128, msg, args...);
// 	Terminal::TermDebugPuts(buf);
// }