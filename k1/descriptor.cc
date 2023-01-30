
#include "descriptor.h"

TaskDescriptor::TaskDescriptor() { }

/**
 * initialization,
 */
TaskDescriptor::TaskDescriptor(int id, int parent_id, int priority, void (*pc)())
	: task_id { id }
	, parent_id { parent_id }
	, priority { priority }
	, prepared_response { 0x0 }
	, alive { true }
	, initialized { false }
	, pc { pc }
{
	sp = (char*)&kernel_stack[4096]; // aligned to 8 bytes, exactly 4kb is used for user stack
}

/**
 * good way to check if a process is still alive
 */
bool TaskDescriptor::is_alive()
{
	return alive;
}

/**
 * Indicating you are killing a process
 */
bool TaskDescriptor::kill()
{
	if (alive)
	{
		alive = false;
		return true;
	}
	return false;
}

/**
 * Debug function used to print out helpful state of a Task descriptor
 * note that this need an update to fill all missing fields
 */
void TaskDescriptor::show_info()
{
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
	print_int(priority);
	print("\r\n", 2);
	char m5[] = "initialized:";
	uart_puts(0, 0, m5, sizeof(m5) - 1);
	print_int((uint64_t)initialized);
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
}