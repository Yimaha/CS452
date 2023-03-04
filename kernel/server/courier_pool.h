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
	void request(T* req, int len);
	void receive(int tid);
	~CourierPool();

private:
	void (*f)();
	etl::queue<int, POOL_SIZE> courier_queue;
};

template <typename T>
void CourierPool<T>::request(T* req, int len) {
	kernel_assert(!courier_queue.empty());
	Message::Send::Send(courier_queue.front(), (const char*)req, len, nullptr, 0);
	courier_queue.pop();
}

template <typename T>
void CourierPool<T>::receive(int from) {
	Message::Reply::Reply(from, nullptr, 0);
	courier_queue.push(from);
}

}
