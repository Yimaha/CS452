#pragma once

#include "../etl/list.h"
#include "../interrupt/clock.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
namespace Courier
{

const uint64_t POOL_SIZE = 16;

template <typename T>
class CourierPool {
public:
	CourierPool(void (*function)(), Priority priority)
		: f { function } {
		for (uint64_t i = 0; i < POOL_SIZE; i++) {
			courier_queue.push(Task::Create(priority, function));
		}
	};
	void request(T* req);
	void receive(int tid);
	~CourierPool();

private:
	void (*f)();
	etl::queue<int, POOL_SIZE> courier_queue;
};

template <typename T>
void CourierPool<T>::request(T* req) {
	if (courier_queue.empty() || courier_queue.front() == -1) {
		Task::_KernelCrash("\r\nout of courier for type: %s\r\n");
	}
	Message::Send::SendNoReply(courier_queue.front(), (const char*)req, sizeof(T));
	courier_queue.pop();
}

template <typename T>
void CourierPool<T>::receive(int from) {
	Message::Reply::EmptyReply(from);
	courier_queue.push(from);
}

}
