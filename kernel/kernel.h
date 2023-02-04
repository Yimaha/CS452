

#pragma once

#define USER_TASK_START_ADDRESS 0x10000000	// dedicated user task space
#define USER_TASK_LIMIT 100					// dedicated amount of user task
#define MAX_NAME_LENGTH 16					// max length of a name
#define NAME_REQ_LENGTH MAX_NAME_LENGTH + 8 // max length of a name request

#define MAIDENLESS -1	 // aka, no parent task
#define NAME_SERVER_ID 1 // the name server is always task 1

#include <new>
#include <stdint.h>

#include "context_switch.h"
#include "descriptor.h"
#include "rpi.h"
#include "scheduler.h"
#include "user/name_server.h"
#include "user/user_tasks_k1.h"
#include "user/user_tasks_k2.h"
#include "user/user_tasks_k2_performance.h"
#include "utils/slab_allocator.h"

namespace Info
{
int MyTid();
int MyParentTid();
}

namespace Destruction
{
void Exit();
// potentially destroy in the future
}

void Yield(); // since it is more like a debug functin, it is consider as "else" namespace

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

/**
 * Kernel state class, stores important information about the kernel and control the flow
 * */
class Kernel {
public:
	enum HandlerCode { NONE = 0, CREATE = 1, MY_TID = 2, MY_PARENT_ID = 3, YIELD = 4, EXIT = 5, SEND = 6, RECEIVE = 7, REPLY = 8, REGISTER_AS = 9, WHO_IS = 10 };
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
