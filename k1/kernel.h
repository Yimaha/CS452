

#pragma once

#define USER_TASK_START_ADDRESS 0x10000000 // dedicated user task space
#define USER_TASK_LIMIT 100 // dedicated amount of user task
#include <new>
#include <stdint.h>

#include "context_switch.h"
#include "descriptor.h"
#include "scheduler.h"
#include "slab_allocator.h"
#include "user_tasks_k1.h"

/**
 * User communication interface, all user tasks should use these functions to talk to the kernel
 * to signify the importance as these all trigger context switches,
 * note that they follow Capitalized Camel Case
 * unlike all other functions within our code
 */
int Create(int priority, void (*function)());
int MyTid();
int MyParentTid();
void Yield();
void Exit();

/**
 * A reconstruction of the user stack registers into a readable struct,
 * both by human and machine, and extractable
 * uint64_t's that are stored on the stack.
 *
 * When used, we simply cast addresses given to us into this struct and extract varables accordingly
 * Credit: https://krinkinmu.github.io/2021/01/10/aarch64-interrupt-handling.html
 */
struct InterruptFrame
{
	uint64_t x0;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
	uint64_t x5;
	uint64_t x6;
	uint64_t x7;
	uint64_t x8;
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;
	uint64_t x16;
	uint64_t x17;
	uint64_t x18;
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t fp;
	uint64_t lr;
	uint64_t xzr;
};

/**
 * Kernel state class, stores important information about the kernel and control the flow
 * */
class Kernel
{
public:
	enum HandlerCode
	{
		NONE = 0,
		CREATE = 1,
		MY_TID = 2,
		MY_PARENT_ID = 3,
		YIELD = 4,
		EXIT = 5
	};
	Kernel();
	~Kernel();
	void schedule_next_task();
	void activate();
	void handle();

private:
    int p_id_counter = 0;                     // keeps track of new task creation id
    int active_task = 0;                      // keeps track of the active_task id
    InterruptFrame *active_request = nullptr; // a storage that saves the active user request
    Scheduler scheduler;                      // scheduler doesn't hold the actual task descriptor,
                                              // simply an id and the priority

	TaskDescriptor* tasks[USER_TASK_LIMIT]; // points to the starting location of taskDescriptors

	// define the type, and follow by the contrustor variable you want to pass to i
	SlabAllocator<TaskDescriptor, int, int, int, void (*)()> task_allocator = SlabAllocator<TaskDescriptor, int, int, int, void (*)()>((char*)USER_TASK_START_ADDRESS, USER_TASK_LIMIT);

	void allocate_new_task(int parent_id, int priority, void (*pc)()); // create, and push a new task onto the actual scheduler
	void queue_task(); // support function that queues the active task only if the active task is not dead

protected:
	// all debug function will either be public or protected, so in future
	// if we need to write any sort of unit test, unit test can have access to the class function
	void check_tasks(int task_id); // check the state of a particular task descriptor
};
