

#pragma once

#define USER_TASK_START_ADDRESS 0x10000000 // dedicated user task space
#define USER_TASK_LIMIT 100				   // dedicated amount of user task

#define MAIDENLESS -1

#include <new>
#include <stdint.h>

#include "context_switch.h"
#include "descriptor.h"
#include "rpi.h"
#include "scheduler.h"
#include "user/user_tasks_k1.h"
#include "user/user_tasks_k2_performance.h"
#include "utils/slab_allocator.h"



/**
 * Kernel state class, stores important information about the kernel and control the flow
 * */
class Kernel {
public:
	enum HandlerCode { NONE = 0, CREATE = 1, MY_TID = 2, MY_PARENT_ID = 3, YIELD = 4, EXIT = 5, SEND = 6, RECEIVE = 7, REPLY = 8 };
	Kernel();
	~Kernel();
	void schedule_next_task();
	void activate();
	void handle();

private:
	int p_id_counter = 0;					  // keeps track of new task creation id
	int active_task = 0;					  // keeps track of the active_task id
	InterruptFrame* active_request = nullptr; // a storage that saves the active user request
	Scheduler scheduler;					  // scheduler doesn't hold the actual task descriptor,
											  // simply an id and the priority

	TaskDescriptor* tasks[USER_TASK_LIMIT] = { nullptr }; // points to the starting location of taskDescriptors, default all nullptr

	// define the type, and follow by the contrustor variable you want to pass to i
	SlabAllocator<TaskDescriptor, int, int, int, void (*)()> task_allocator = SlabAllocator<TaskDescriptor, int, int, int, void (*)()>((char*)USER_TASK_START_ADDRESS, USER_TASK_LIMIT);

	void allocate_new_task(int parent_id, int priority, void (*pc)()); // create, and push a new task onto the actual scheduler
	void handle_send();
	void handle_receive();
	void handle_reply();

protected:
	// all debug function will either be public or protected, so in future
	// if we need to write any sort of unit test, unit test can have access to the class function
	void check_tasks(int task_id); // check the state of a particular task descriptor
};
/**
 * User communication interface, all user tasks should use these functions to talk to the kernel
 * to signify the importance as these all trigger context switches,
 * note that they follow Capitalized Camel Case
 * unlike all other functions within our code
 */
namespace Task
{
namespace Creation
{
	static inline int Create(int priority, void (*function)()) {
		return to_kernel(Kernel::HandlerCode::CREATE, priority, function);
	}
}

namespace Info
{
	static inline int MyTid() {
		return to_kernel(Kernel::HandlerCode::MY_TID);
	}
	static inline int MyParentTid() {
		return to_kernel(Kernel::HandlerCode::MY_PARENT_ID);
	}
}

namespace Destruction
{
	static inline void Exit() {
		to_kernel(Kernel::HandlerCode::EXIT);
	}
	// potentially destroy in the future
}

static inline void Yield() {
	to_kernel(Kernel::HandlerCode::YIELD);
} // since it is more like a debug functin, it is consider as "else" namespace
}

namespace MessagePassing
{
namespace Send
{
	static inline int Send(int tid, const char* msg, int msglen, char* reply, int rplen) {
		return to_kernel(Kernel::HandlerCode::SEND, tid, msg, msglen, reply, rplen);
	}
	enum Exception { NO_SUCH_TASK = -1, CANNOT_BE_COMPLETE = -2 };
}

namespace Receive
{
	static inline int Receive(int* tid, char* msg, int msglen) {
		return to_kernel(Kernel::HandlerCode::RECEIVE, tid, msg, msglen);
	}
}

namespace Reply
{
	static inline int Reply(int tid, const char* msg, int msglen) {
		return to_kernel(Kernel::HandlerCode::REPLY, tid, msg, msglen);

	}
	enum Exception { NO_SUCH_TASK = -1, NOT_WAITING_FOR_REPLY = -2 };
}
}
