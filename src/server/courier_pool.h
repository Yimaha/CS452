#pragma once

#include "../etl/list.h"
#include "../interrupt/clock.h"
#include "../kernel.h"
#include "../rpi.h"
#include "../utils/utility.h"
namespace Courier
{

const int REQUEST_SIZE = 64;

template <typename T, uint64_t POOL_SIZE = 4>
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
	etl::queue<T, REQUEST_SIZE> request_queue;
};

template <typename T, uint64_t POOL_SIZE>
void CourierPool<T, POOL_SIZE>::request(T* req) {
	if (courier_queue.empty()) {
		request_queue.push(*req);
	} else {
		Message::Send::SendNoReply(courier_queue.front(), (const char*)req, sizeof(T));
		courier_queue.pop();
	}
}

template <typename T, uint64_t POOL_SIZE>
void CourierPool<T, POOL_SIZE>::receive(int from) {
	Message::Reply::EmptyReply(from);
	courier_queue.push(from);
	if (!request_queue.empty()) {
		Message::Send::SendNoReply(courier_queue.front(), (const char*)&request_queue.front(), sizeof(T));
		request_queue.pop();
		courier_queue.pop();
	}
}

}
